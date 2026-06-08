from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def test_plugin_registers_glassveil_filter():
    source = read("src/plugin-main.c")
    assert "OBS_SOURCE_TYPE_FILTER" in source
    assert "obs_register_source(&glassveil_filter)" in source
    assert "glassveil_filter_render" in source


def test_plugin_exposes_expected_properties():
    source = read("src/plugin-main.c")
    for prop in [
        "capture_background",
        "glass_amount",
        "distortion",
        "liquid_speed",
        "edge_glow",
        "chromatic_split",
        "highlight_strength",
        "mix_amount",
        "cloak_amount",
        "scene_through",
        "scene_through_amount",
        "mask_threshold",
        "mask_softness",
        "debug_mask",
    ]:
        assert prop in source
    assert "obs_properties_add_button" in source


def test_shader_asset_contains_glassveil_uniforms():
    shader = read("data/effects/glassveil.effect")
    for uniform in [
        "image",
        "background_image",
        "has_background",
        "texture_width",
        "texture_height",
        "elapsed_time",
        "glass_amount",
        "distortion",
        "edge_glow",
        "chromatic_split",
        "highlight_strength",
        "mix_amount",
        "cloak_amount",
        "scene_through",
        "scene_through_amount",
        "mask_threshold",
        "mask_softness",
        "debug_mask",
    ]:
        assert uniform in shader
    assert "saturate(cloak_amount)" not in shader
    assert "uniform float2 texture_size =" not in shader
    assert "return float4(final_color, output_alpha)" in shader
    assert "scene_edge_alpha" in shader


def test_scene_through_mode_is_default_off_and_documented():
    source = read("src/plugin-main.c")
    locale = read("data/locale/en-US.ini")
    quickstart = read("docs/OBS-QUICKSTART.md")

    assert 'obs_data_set_default_bool(settings, "scene_through", false)' in source
    assert "SceneThrough" in locale
    assert "Scene-through overlay" in quickstart


def test_plugin_uses_background_plate_render_target():
    source = read("src/plugin-main.c")
    assert "gs_texrender_t *background_render" in source
    assert "capture_requested" in source
    assert "gs_effect_set_texture" in source
    assert "background_image" in source


def test_cmake_installs_effect_asset():
    cmake = read("CMakeLists.txt")
    assert "data/effects/glassveil.effect" in cmake
    assert "target_sources" in cmake


def test_windows_release_scripts_are_present_and_guarded():
    package_script = read("tools/package-windows.ps1")
    install_script = read("tools/install-windows.ps1")
    uninstall_script = read("tools/uninstall-windows.ps1")

    for required_path in [
        "bin\\64bit\\obs-glassveil.dll",
        "data\\effects\\glassveil.effect",
        "data\\locale\\en-US.ini",
    ]:
        assert required_path in install_script

    assert "Get-Process -Name \"obs64\"" in install_script
    assert "Close OBS before installing Glassveil" in install_script
    assert "Compress-Archive" in package_script
    assert "OBS-QUICKSTART.md" in package_script
    assert "RELEASE_NOTES.md" in package_script
    assert "CONTRIBUTING.md" in package_script
    assert "NOTICE.md" in package_script
    assert "Remove-Item" in uninstall_script


def test_user_handoff_docs_cover_clean_plate_workflow():
    quickstart = read("docs/OBS-QUICKSTART.md")
    release_notes = read("RELEASE_NOTES.md")
    readme = read("README.md")
    notice = read("NOTICE.md")

    for doc in [quickstart, readme]:
        assert "Capture Clean Plate" in doc
        assert "Debug Mask" in doc
        assert "Mask Threshold" in doc

    assert "clean-plate differencing" in release_notes
    assert "AI/semantic person segmentation" in release_notes
    assert "obs-plugintemplate" in notice
