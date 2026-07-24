# Portal 2 VR Mod — v5.2

Open-source VR mod for Portal 2. Motion controls, 6DoF, room-scale, grabbable objects.  
Built on the L4D2VR framework with all fixes and performance optimizations applied.

## What's new in v5.2

- All dead code removed, unused hooks cleaned up
- Grabbable-object physics (ComputeError, RotateObject) enabled
- Performance: arms model caching, consolidated render context, state-transition input (no redundant commands), cached sin/cos, no render context leaks
- Builds clean with 0 errors
- Correct deployment structure matching original 0.2.0 layout

## Zip contents

```
Portal2VR_v5.2.zip
├── bin\
│   └── d3d9.dll              ← VR mod (replace Portal 2's original)
├── VR\
│   ├── config.txt             ← settings file
│   ├── manifest.vrmanifest    ← SteamVR app registration
│   ├── portal2vr_capsule_main.png
│   ├── portal2vr_portrait_main.png
│   └── SteamVRActionManifest\
│       ├── action_manifest.json
│       ├── bindings_knuckles.json
│       ├── bindings_oculus_touch.json
│       └── bindings_vive_cosmos_controller.json
└── README.md
```

## How to install

1. **Back up** `steamapps\common\Portal 2\bin\d3d9.dll` (rename or copy elsewhere)
2. Extract **all contents** of `Portal2VR_v5.2.zip` into `steamapps\common\Portal 2`
3. Let it merge folders — `bin\` merges with the game's `bin\`, `VR\` is a new folder

## How to launch

1. Connect headset, start **SteamVR**
2. In Steam, right-click Portal 2 → Properties → Launch Options, paste:
   ```
   -insecure -window -novid +mat_motion_blur_percent_of_screen_max 0 +mat_queue_mode 0 +mat_vsync 0 +mat_antialias 0 +mat_grain_scale_override 0 -width 1280 -height 720
   ```
3. Launch Portal 2
4. At the menu set video options to:
   - **Display mode**: Fullscreen (windowed)
   - **Model detail**: High
5. Load into a chapter

The mod activates automatically when SteamVR is detected. No toggle needed.

## Controls

| Action | Default Binding |
|---|---|
| Primary fire (blue portal) | Right trigger |
| Secondary fire (orange portal) | Left trigger |
| Jump | A / Right touchpad press |
| Crouch | B / Left touchpad press |
| Use / Grab objects | Grip button |
| Reload | X / Y |
| Walk | Left thumbstick |
| Turn | Right thumbstick (left/right) |
| Recenter camera | Left stick press |
| Flashlight | Stick button |
| Next / Previous item | D-pad up / down |
| Reset VR position | Stick press |

## Config

Edit `Portal 2\VR\config.txt`:

| Setting | Values | Default | Description |
|---|---|---|---|
| `AimMode` | 0, 1, 2 | 2 | 0=off, 1=crosshair, 2=laser sight |
| `SnapTurning` | true/false | false | Snap turn instead of smooth |
| `SnapTurnAngle` | float | 45.0 | Degrees per snap turn |
| `LeftHanded` | true/false | false | Swap controller roles |
| `TurnSpeed` | float | 0.15 | Smooth turn speed |
| `VRScale` | float | 43.2 | World scale factor |
| `IPDScale` | float | 1.0 | Interpupillary distance multiplier |
| `6DOF` | true/false | true | Enable positional tracking |
| `AntiAliasing` | 0, 2, 4, 8 | 0 | MSAA level |
| `RenderWindow` | 0/1 | 0 | Render a 3rd view for the desktop window (expensive) |
| `ViewmodelPosCustomOffsetX/Y/Z` | float | 0.0 | Custom viewmodel position offset |
| `ViewmodelAngCustomOffsetX/Y/Z` | float | 0.0 | Custom viewmodel angle offset |

## Troubleshooting

| Problem | Fix |
|---|---|
| No audio | Run `Portal 2\portal2_dlc3\UpdateSoundCache.cmd` |
| Not loading in VR | Start SteamVR before the game; disable SteamVR Theater mode |
| Stuttering | Steam Settings → Shader Pre-Caching → Allow background Vulkan shaders |
| Crashing | Lower video settings, disable add-ons, verify game files, reinstall |

## Credits

Based on [l4d2vr](https://github.com/sd805/l4d2vr).  
Uses code from VirtualFortress2, gmcl_openvr, dxvk (vr-dx9-rel), source-sdk-2013.
