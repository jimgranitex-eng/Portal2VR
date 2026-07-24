# Portal 2 VR Mod

**Current Release: v5.2** — July 24, 2026  
Open-source VR mod for Portal 2. Motion controls, 6DoF, room-scale, grabbable objects.

Built on the L4D2VR framework with all fixes, dead code removal, and performance optimizations applied.

## How to Launch (Important — Read Carefully)

**This is NOT a separate SteamVR app.** The mod works by replacing Portal 2's `d3d9.dll`. You launch Portal 2 normally from Steam — the mod auto-detects SteamVR and switches to VR mode.

### Step-by-step

1. **Download** [Portal2VR_v5.2.zip](./Portal2VR_v5.2.zip) and extract into `steamapps\common\Portal 2` (merge folders)

2. **Connect your headset** and **start SteamVR**

3. **Set launch options** — In Steam, right-click Portal 2 → Properties → Launch Options, paste:
   ```
   -insecure -window -novid +mat_motion_blur_percent_of_screen_max 0 +mat_queue_mode 0 +mat_vsync 0 +mat_antialias 0 +mat_grain_scale_override 0 -width 1280 -height 720
   ```

4. **Launch Portal 2** from your Steam library (same as always)

5. **The mod activates automatically** — SteamVR will show the game in your headset. No toggle needed.

> **If it doesn't work:** Make sure SteamVR is running BEFORE you launch Portal 2. Disable SteamVR Theater mode in Steam settings.

### To go back to normal Portal 2

Delete the `VR\` folder from Portal 2 and restore your original `bin\d3d9.dll` backup.

## Repository Structure

```
Portal2VR/
├── L4D2VR/                  Source code (current v5.2)
├── dxvk/                    D3D9-to-Vulkan bridge (submodule)
├── thirdparty/              Dependencies (MinHook, OpenVR)
├── imgs/                    Images
├── archive/
│   └── v0.2.0/              Original 0.2.0 release (reference)
├── Portal2VR_v5.2.zip       Pre-built release package
├── l4d2vr.sln               Visual Studio solution
└── README.md                This file
```

## v5.2 Improvements over v0.2.0

| Category | v0.2.0 (Original) | v5.2 (Current) |
|---|---|---|
| Dead code | ~50 lines dead typedefs, hooks, offsets | All removed |
| Render context leaks | 4 leaks per frame | 0 leaks |
| Per-frame allocations | `std::string` + `find` every model draw | `strstr` + cache guard |
| Input handling | `+attack`/`-attack` every frame (12 calls/frame) | Only on state transitions |
| Grabbable objects | ComputeError + RotateObject disabled | Enabled |
| Build | Untested | Clean 0 errors Release x86 |
| Documentation | Basic README | Full guide with controls, config, troubleshooting |

## Credits

Based on [l4d2vr](https://github.com/sd805/l4d2vr). Uses code from VirtualFortress2, gmcl_openvr, dxvk (vr-dx9-rel), source-sdk-2013.
