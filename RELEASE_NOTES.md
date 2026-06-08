# Glassveil Release Notes

## 0.1.1-alpha

Added:

- Optional `Scene-through overlay` mode.
- `Scene-through Amount` control for letting lower OBS layers show through the foreground mask.
- Quickstart instructions for layering a screen, game, or video source under the webcam.

## 0.1.0-alpha

Glassveil is an OBS effect filter for a liquid-glass webcam cloak.

Included:

- Native OBS effect filter named `Glassveil`.
- Liquid refraction, edge glow, chromatic split, and highlight controls.
- `Capture Clean Plate` background capture workflow.
- Clean-plate foreground differencing for see-through cloak mode.
- Optional `Scene-through overlay` mode so lower OBS layers can show through the foreground mask.
- `Debug Mask` tuning view.
- Windows x64 release ZIP layout for OBS manual installation.
- PowerShell package, install, and uninstall scripts.

Known limitations:

- Cloaking uses clean-plate differencing, not AI/semantic person segmentation.
- Lighting changes, camera auto-exposure, or moved background objects can reduce mask quality.
- Windows x64 is the only packaged build in this local alpha.
