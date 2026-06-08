# Contributing to Glassveil

Glassveil is a working alpha OBS plugin. The current goal is to keep the core liquid-glass cloak usable while making it easier for people to tune, port, package, and improve.

Good contribution areas:

- Shader tuning presets that preserve the current default look.
- Better clean-plate masking and edge cleanup.
- macOS and Linux packaging.
- OBS UI polish that does not clutter the filter settings.
- Documentation, screenshots, and tuning recipes.

Please keep changes small and testable. If a change alters the visual behavior, describe what changed and include before/after notes or screenshots.

Before submitting changes, run:

```powershell
python -m pytest tests\validate_plugin.py -q
cmake --build --preset windows-x64
```
