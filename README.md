# Glassveil for OBS

Glassveil is a working alpha OBS effect filter that gives a camera or video source a liquid-glass cloak. It began as a quick visual prototype and is now a functioning native OBS plugin, but it should still be treated as prototype-level software.

Important: every Glassveil session starts by capturing a clean backplate. After loading the filter and opening its settings, step fully out of frame and click `Capture Clean Plate` before tuning the cloak.

This repository is based on the official OBS plugin template and is intended to become the installable plugin that appears in:

`Camera Source -> Filters -> Effect Filters -> Glassveil`

## Current Status

Implemented:

- Native OBS effect filter registration.
- `Glassveil` filter name and localization strings.
- OBS properties for the main Glassveil controls.
- GPU effect shader asset.
- Clean-plate capture button for see-through cloak mode.
- Two-texture render path for live frame plus captured background.
- Cross-platform OBS plugin-template structure.
- Windows x64 release ZIP with install/uninstall scripts and quickstart docs.

Not implemented yet:

- macOS/Linux release artifacts.
- AI/semantic person segmentation. Cloaking currently uses clean-plate differencing.

The browser prototype proved the cloak workflow. The native OBS filter now has its own render-target/background-texture pass, so `Cloak Amount`, `Mask Threshold`, and `Mask Softness` are active when a clean plate has been captured.

## Controls

- Glass
- Distortion
- Liquid Speed
- Edge Glow
- Chromatic Split
- Highlight Strength
- Mix
- Capture Clean Plate
- Cloak Amount
- Scene-through overlay
- Scene-through Amount
- Mask Threshold
- Mask Softness
- Debug Mask

To use cloaking, step out of the camera view and click `Capture Clean Plate`. Step back in, then tune `Cloak Amount`, `Mask Threshold`, and `Mask Softness`. Use `Debug Mask` to see which pixels the filter thinks are foreground.

To let another OBS source show through you, put that source below your webcam source, then enable `Scene-through overlay` on the Glassveil filter. It uses the same clean-plate mask, but makes the foreground region transparent instead of only replacing it with the captured room plate.

## Build

This project uses the official `obs-plugintemplate` CMake workflow.

On Windows, install:

- OBS Studio
- Visual Studio 2022 with Desktop development with C++
- CMake 3.28 or newer
- Git

Then from this directory:

```powershell
cmake --preset windows-x64
cmake --build --preset windows-x64
```

If the preset downloads OBS dependencies successfully, the built plugin will be in the template's build/output folders.

## Install for Local OBS Testing

After building, package and install the Windows build:

```powershell
.\tools\package-windows.ps1
.\tools\install-windows.ps1
```

The recommended Windows manual plugin directory is:

```text
C:\ProgramData\obs-studio\plugins\obs-glassveil
  bin\64bit\obs-glassveil.dll
  data\effects\glassveil.effect
  data\locale\en-US.ini
```

Restart OBS, then:

1. Select your camera source.
2. Open `Filters`.
3. Under `Effect Filters`, click `+`.
4. Choose `Glassveil`.

See `docs/OBS-QUICKSTART.md` for the clean-plate workflow and troubleshooting steps.

## Development Notes

The current filter renders the source into plugin-owned `gs_texrender_t` textures:

1. Render the incoming camera/source frame into a texture.
2. Capture/store a background texture when the user clicks `Capture Clean Plate`.
3. Pass both the live source texture and background texture into the shader.
4. Use a foreground/background difference mask to apply the cloak only to the subject.

That is the native OBS equivalent of the browser prototype's `Capture BG` workflow.

## Attribution

Glassveil is based on the official OBS plugin template:

https://github.com/obsproject/obs-plugintemplate

OBS Studio and the OBS plugin template are created and maintained by the OBS Project contributors.
