/*
Glassveil for OBS
Copyright (C) 2026 Glassveil contributors

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include <obs-module.h>
#include <plugin-support.h>

#include <graphics/effect.h>
#include <graphics/graphics.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

#define SET_FLOAT(effect, name, value)                 \
	do {                                           \
		gs_eparam_t *param =                  \
			gs_effect_get_param_by_name(effect, name); \
		if (param)                             \
			gs_effect_set_float(param, value); \
	} while (false)

struct glassveil_filter {
	obs_source_t *source;
	gs_effect_t *effect;
	gs_texrender_t *frame_render;
	gs_texrender_t *background_render;
	float elapsed_time;
	float glass_amount;
	float distortion;
	float liquid_speed;
	float edge_glow;
	float chromatic_split;
	float highlight_strength;
	float mix_amount;
	float cloak_amount;
	float scene_through_amount;
	float mask_threshold;
	float mask_softness;
	uint32_t background_width;
	uint32_t background_height;
	bool capture_requested;
	bool has_background;
	bool scene_through;
	bool debug_mask;
};

static const char *glassveil_filter_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("Glassveil");
}

static void glassveil_filter_update(void *data, obs_data_t *settings)
{
	struct glassveil_filter *filter = data;

	filter->glass_amount = (float)obs_data_get_double(settings, "glass_amount");
	filter->distortion = (float)obs_data_get_double(settings, "distortion");
	filter->liquid_speed = (float)obs_data_get_double(settings, "liquid_speed");
	filter->edge_glow = (float)obs_data_get_double(settings, "edge_glow");
	filter->chromatic_split = (float)obs_data_get_double(settings, "chromatic_split");
	filter->highlight_strength = (float)obs_data_get_double(settings, "highlight_strength");
	filter->mix_amount = (float)obs_data_get_double(settings, "mix_amount");
	filter->cloak_amount = (float)obs_data_get_double(settings, "cloak_amount");
	filter->scene_through_amount = (float)obs_data_get_double(settings, "scene_through_amount");
	filter->mask_threshold = (float)obs_data_get_double(settings, "mask_threshold");
	filter->mask_softness = (float)obs_data_get_double(settings, "mask_softness");
	filter->scene_through = obs_data_get_bool(settings, "scene_through");
	filter->debug_mask = obs_data_get_bool(settings, "debug_mask");
}

static void glassveil_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "glass_amount", 0.94);
	obs_data_set_default_double(settings, "distortion", 0.72);
	obs_data_set_default_double(settings, "liquid_speed", 0.88);
	obs_data_set_default_double(settings, "edge_glow", 0.54);
	obs_data_set_default_double(settings, "chromatic_split", 0.44);
	obs_data_set_default_double(settings, "highlight_strength", 0.72);
	obs_data_set_default_double(settings, "mix_amount", 1.0);
	obs_data_set_default_double(settings, "cloak_amount", 1.0);
	obs_data_set_default_bool(settings, "scene_through", false);
	obs_data_set_default_double(settings, "scene_through_amount", 1.0);
	obs_data_set_default_double(settings, "mask_threshold", 0.22);
	obs_data_set_default_double(settings, "mask_softness", 0.13);
	obs_data_set_default_bool(settings, "debug_mask", false);
}

static bool glassveil_capture_background(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);

	struct glassveil_filter *filter = data;
	if (filter)
		filter->capture_requested = true;

	return false;
}

static obs_properties_t *glassveil_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_button(props, "capture_background", obs_module_text("CaptureBackground"),
				  glassveil_capture_background);

	obs_properties_add_float_slider(props, "glass_amount", obs_module_text("GlassAmount"), 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, "distortion", obs_module_text("Distortion"), 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, "liquid_speed", obs_module_text("LiquidSpeed"), 0.0, 3.0, 0.01);
	obs_properties_add_float_slider(props, "edge_glow", obs_module_text("EdgeGlow"), 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, "chromatic_split", obs_module_text("ChromaticSplit"), 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, "highlight_strength", obs_module_text("HighlightStrength"), 0.0, 1.0,
					0.01);
	obs_properties_add_float_slider(props, "mix_amount", obs_module_text("MixAmount"), 0.0, 1.0, 0.01);

	obs_properties_add_float_slider(props, "cloak_amount", obs_module_text("CloakAmount"), 0.0, 1.0, 0.01);
	obs_properties_add_bool(props, "scene_through", obs_module_text("SceneThrough"));
	obs_properties_add_float_slider(props, "scene_through_amount", obs_module_text("SceneThroughAmount"), 0.0, 1.0,
					0.01);
	obs_properties_add_float_slider(props, "mask_threshold", obs_module_text("MaskThreshold"), 0.0, 1.0, 0.01);
	obs_properties_add_float_slider(props, "mask_softness", obs_module_text("MaskSoftness"), 0.01, 0.8, 0.01);
	obs_properties_add_bool(props, "debug_mask", obs_module_text("DebugMask"));

	UNUSED_PARAMETER(data);
	return props;
}

static void *glassveil_filter_create(obs_data_t *settings, obs_source_t *source)
{
	struct glassveil_filter *filter = bzalloc(sizeof(*filter));
	char *effect_path = obs_module_file("effects/glassveil.effect");
	char *error_string = NULL;

	filter->source = source;

	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, &error_string);
	filter->frame_render = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	filter->background_render = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	obs_leave_graphics();

	if (!filter->effect) {
		obs_log(LOG_ERROR, "failed to load effect: %s", error_string ? error_string : "unknown error");
	}

	bfree(effect_path);
	bfree(error_string);

	glassveil_filter_update(filter, settings);
	return filter;
}

static void glassveil_filter_destroy(void *data)
{
	struct glassveil_filter *filter = data;

	obs_enter_graphics();
	if (filter->effect)
		gs_effect_destroy(filter->effect);
	if (filter->frame_render)
		gs_texrender_destroy(filter->frame_render);
	if (filter->background_render)
		gs_texrender_destroy(filter->background_render);
	obs_leave_graphics();

	bfree(filter);
}

static void glassveil_filter_tick(void *data, float seconds)
{
	struct glassveil_filter *filter = data;
	filter->elapsed_time += seconds;
}

static bool render_source_to_texture(gs_texrender_t *render, obs_source_t *target, obs_source_t *parent, uint32_t width,
				     uint32_t height)
{
	if (!render)
		return false;

	gs_texrender_reset(render);
	if (!gs_texrender_begin(render, width, height))
		return false;

	gs_blend_state_push();
	gs_blend_function_separate(GS_BLEND_SRCALPHA, GS_BLEND_INVSRCALPHA, GS_BLEND_ONE, GS_BLEND_INVSRCALPHA);

	struct vec4 clear_color;
	vec4_zero(&clear_color);
	gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
	gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);

	uint32_t parent_flags = obs_source_get_output_flags(parent);
	bool custom_draw = (parent_flags & OBS_SOURCE_CUSTOM_DRAW) != 0;
	bool async = (parent_flags & OBS_SOURCE_ASYNC) != 0;

	if (target == parent && !custom_draw && !async)
		obs_source_default_render(target);
	else
		obs_source_video_render(target);

	gs_blend_state_pop();
	gs_texrender_end(render);

	return gs_texrender_get_texture(render) != NULL;
}

static void render_glassveil_texture(struct glassveil_filter *filter, gs_texture_t *frame_texture,
				     gs_texture_t *background_texture, uint32_t width, uint32_t height)
{
	gs_eparam_t *image = gs_effect_get_param_by_name(filter->effect, "image");
	gs_eparam_t *background_image = gs_effect_get_param_by_name(filter->effect, "background_image");

	if (image)
		gs_effect_set_texture(image, frame_texture);
	if (background_image)
		gs_effect_set_texture(background_image, background_texture ? background_texture : frame_texture);

	SET_FLOAT(filter->effect, "elapsed_time", filter->elapsed_time);
	SET_FLOAT(filter->effect, "glass_amount", filter->glass_amount);
	SET_FLOAT(filter->effect, "distortion", filter->distortion);
	SET_FLOAT(filter->effect, "liquid_speed", filter->liquid_speed);
	SET_FLOAT(filter->effect, "edge_glow", filter->edge_glow);
	SET_FLOAT(filter->effect, "chromatic_split", filter->chromatic_split);
	SET_FLOAT(filter->effect, "highlight_strength", filter->highlight_strength);
	SET_FLOAT(filter->effect, "mix_amount", filter->mix_amount);
	SET_FLOAT(filter->effect, "cloak_amount", filter->cloak_amount);
	SET_FLOAT(filter->effect, "scene_through", filter->scene_through ? 1.0f : 0.0f);
	SET_FLOAT(filter->effect, "scene_through_amount", filter->scene_through_amount);
	SET_FLOAT(filter->effect, "mask_threshold", filter->mask_threshold);
	SET_FLOAT(filter->effect, "mask_softness", filter->mask_softness);
	SET_FLOAT(filter->effect, "debug_mask", filter->debug_mask ? 1.0f : 0.0f);
	SET_FLOAT(filter->effect, "has_background", background_texture ? 1.0f : 0.0f);
	SET_FLOAT(filter->effect, "texture_width", (float)width);
	SET_FLOAT(filter->effect, "texture_height", (float)height);

	gs_technique_t *tech = gs_effect_get_technique(filter->effect, "Draw");
	size_t passes = gs_technique_begin(tech);
	for (size_t i = 0; i < passes; i++) {
		gs_technique_begin_pass(tech, i);
		gs_draw_sprite(frame_texture, 0, width, height);
		gs_technique_end_pass(tech);
	}
	gs_technique_end(tech);
}

static void glassveil_filter_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);

	struct glassveil_filter *filter = data;
	obs_source_t *target = obs_filter_get_target(filter->source);
	obs_source_t *parent = obs_filter_get_parent(filter->source);

	if (!target || !parent || !filter->effect) {
		obs_source_skip_video_filter(filter->source);
		return;
	}

	uint32_t width = obs_source_get_base_width(target);
	uint32_t height = obs_source_get_base_height(target);

	if (!width || !height) {
		obs_source_skip_video_filter(filter->source);
		return;
	}

	if (filter->has_background && (filter->background_width != width || filter->background_height != height)) {
		filter->has_background = false;
	}

	if (!render_source_to_texture(filter->frame_render, target, parent, width, height)) {
		obs_source_skip_video_filter(filter->source);
		return;
	}

	if (filter->capture_requested) {
		filter->capture_requested = false;
		filter->has_background =
			render_source_to_texture(filter->background_render, target, parent, width, height);
		if (filter->has_background) {
			filter->background_width = width;
			filter->background_height = height;
			obs_log(LOG_INFO, "captured clean background plate");
		} else {
			obs_log(LOG_WARNING, "failed to capture clean background plate");
		}
	}

	gs_texture_t *frame_texture = gs_texrender_get_texture(filter->frame_render);
	gs_texture_t *background_texture = filter->has_background ? gs_texrender_get_texture(filter->background_render)
								  : NULL;

	if (!frame_texture) {
		obs_source_skip_video_filter(filter->source);
		return;
	}

	render_glassveil_texture(filter, frame_texture, background_texture, width, height);
}

static struct obs_source_info glassveil_filter = {
	.id = "glassveil_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = glassveil_filter_get_name,
	.create = glassveil_filter_create,
	.destroy = glassveil_filter_destroy,
	.update = glassveil_filter_update,
	.get_defaults = glassveil_filter_defaults,
	.get_properties = glassveil_filter_properties,
	.video_tick = glassveil_filter_tick,
	.video_render = glassveil_filter_render,
};

bool obs_module_load(void)
{
	obs_register_source(&glassveil_filter);
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}
