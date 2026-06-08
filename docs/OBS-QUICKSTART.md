# Glassveil OBS Quickstart

## Step 1: Capture a Clean Plate

Every Glassveil session starts with a clean backplate. After adding or opening the filter settings, step fully out of the camera frame and click `Capture Clean Plate` before tuning the cloak.

## Install

1. Close OBS completely.
2. Extract the release ZIP.
3. Run PowerShell from the extracted folder.
4. Install:

```powershell
.\tools\install-windows.ps1
```

The Windows plugin path is:

```text
C:\ProgramData\obs-studio\plugins\obs-glassveil
```

Restart OBS after installing.

## Add the Filter

1. Select your webcam source.
2. Open `Filters`.
3. Under `Effect Filters`, click `+`.
4. Choose `Glassveil`.

## Cloak Workflow

1. Step fully out of the camera frame.
2. Click `Capture Clean Plate`.
3. Step back into frame.
4. Enable `Debug Mask`.
5. Tune `Mask Threshold` and `Mask Softness` until only your body is highlighted.
6. Disable `Debug Mask`.
7. Tune `Cloak Amount`, `Glass`, and `Distortion`.

## Scene-through Overlay

Use this when you want a game, screen share, or other video source to show through your glass silhouette.

1. Put the screen/video source below your webcam source in OBS.
2. Put your webcam source above it.
3. Add or open the `Glassveil` filter on the webcam source.
4. Step out of frame and click `Capture Clean Plate`.
5. Step back in and enable `Scene-through overlay`.
6. Tune `Scene-through Amount` for how strongly the lower OBS layer shows through you.

This mode still needs a clean plate. It uses the clean plate to find you, then reduces alpha in that foreground region so the layer underneath can show through.

## Tuning Notes

- If too much of the room disappears, raise `Mask Threshold`.
- If too much of you remains visible, lower `Mask Threshold`.
- If the edge is harsh, raise `Mask Softness`.
- If the effect looks too loud, lower `Distortion` or `Edge Glow`.
- If the room lighting changes, capture a new clean plate.

## Troubleshooting

If the filter appears but sliders do nothing, check the newest OBS log in:

```text
%APPDATA%\obs-studio\logs
```

Look for lines containing `obs-glassveil` or `glassveil.effect`. A shader load error means OBS loaded the plugin UI but skipped the effect.

If the filter does not appear at all, confirm these files exist:

```text
C:\ProgramData\obs-studio\plugins\obs-glassveil\bin\64bit\obs-glassveil.dll
C:\ProgramData\obs-studio\plugins\obs-glassveil\data\effects\glassveil.effect
C:\ProgramData\obs-studio\plugins\obs-glassveil\data\locale\en-US.ini
```
