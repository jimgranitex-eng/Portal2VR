# Portal 2 VR Mod

**v5.3.0.8** — *"Loader Lock Fix"* — **July 25, 2026**

[![License](https://img.shields.io/badge/license-GPL--3.0-blue)](LICENSE)
[![Build x86](https://img.shields.io/badge/build-Release%20x86-brightgreen)]()
[![Build x64](https://img.shields.io/badge/build-Release%20x64-brightgreen)]()

Open-source VR mod for Portal 2. Full motion controls, 6DoF tracking, room-scale, grabbable objects.

Works in **single player** and **co-op** — identical code path, zero extra config.

---

## Quick Install (2 minutes)

1. **Download** [`Portal2VR_v5.3.0.8.zip`](./Portal2VR_v5.3.0.8.zip)
2. **Extract** all contents into your Portal 2 install folder:
   ```
   C:\Program Files (x86)\Steam\steamapps\common\Portal 2
   ```
   Merge/overwrite when prompted. After extracting you should have:
   ```
   Portal 2/
   ├── d3d9.dll              (new — VR mod loader)
   ├── bin/d3d9.dll          (new — x64 VR bridge)
   └── VR/
       ├── config.txt        (VR settings)
       ├── manifest.vrmanifest
       ├── action_manifest.json
       └── launch_vr.bat
   ```
3. **Connect your VR headset** and make sure **SteamVR** is running
4. **Launch** one of these ways:
   - **Option A:** Double-click `VR\launch_vr.bat` (recommended — auto-starts SteamVR if needed)
   - **Option B:** Launch Portal 2 from your Steam library normally

That's it. The mod auto-detects SteamVR and applies optimal settings on first launch.

---

## How to Use

### First Launch

1. Make sure SteamVR is running with your headset connected
2. Run `VR\launch_vr.bat` (or launch Portal 2 from Steam)
3. The game window will appear on your desktop; the VR view renders in your headset
4. You should see Portal 2's main menu in VR

### If VR Doesn't Activate

If the game launches but you don't see VR in your headset:
1. Check `Portal 2\VR\portal2vr.log` — it logs every init step with timestamps
2. If VR_Init failed, a **popup error** will appear describing the issue
3. Common fixes:
   - Restart SteamVR, then relaunch the game
   - Make sure your headset is detected in SteamVR settings
   - Check GPU drivers support Vulkan
4. If nothing works, run `VR\register_vr.bat` to re-register with SteamVR

### The VR Log File

The mod writes a detailed init log to:
```
Portal 2\VR\portal2vr.log
```

This log shows every initialization step. If something fails, it will show exactly where and why. The log is overwritten each time you launch the game.

### Configuration

Edit `Portal 2\VR\config.txt` while the game is running — changes apply immediately (live reload).

| Setting | Default | Description |
|---|---|---|
| `AimMode` | 2 | 0=off, 1=crosshair, 2=laser sight |
| `SnapTurning` | false | Snap turn vs smooth |
| `SnapTurnAngle` | 45.0 | Degrees per snap turn |
| `LeftHanded` | false | Swap controller roles |
| `TurnSpeed` | 0.15 | Smooth turn speed multiplier |
| `VRScale` | 43.2 | World scale (adjust for room-scale feel) |
| `IPDScale` | 1.0 | IPD multiplier |
| `6DOF` | true | Positional tracking (off = 3DoF only) |
| `AntiAliasing` | 0 | MSAA level (0, 2, 4, 8) |
| `RenderWindow` | 0 | Desktop mirror window mode |
| `SkipLoadingScreen` | true | Auto-skip loading screens and intro videos |
| `ViewmodelPosCustomOffsetX/Y/Z` | 0 | Viewmodel position offset |
| `ViewmodelAngCustomOffsetX/Y/Z` | 0 | Viewmodel angle offset |

### Controls

| Action | Binding |
|---|---|
| Blue portal | Right trigger |
| Orange portal | Left trigger |
| Jump | A / Right touchpad |
| Crouch | B / Left touchpad |
| Use / Grab | Grip button |
| Reload | X / Y |
| Walk | Left stick |
| Turn | Right stick (smooth or snap) |
| Recenter | Left stick press |
| Flashlight | Stick button |
| Next / Prev item | D-pad up / down |
| Reset VR position | Stick press |

Supported controllers: **Oculus Touch**, **Valve Index (Knuckles)**, **Vive Cosmos**.

---

## What's New in v5.3.0.8

| Feature | v5.3.0.7 (Previous) | v5.3.0.8 (Current) |
|---|---|---|
| DllMain | VRLog called inside DllMain (loader lock) | **VRLog removed from DllMain** — no CRT in loader lock |
| DLL loading | Windows silently unloaded our DLL | **DLL stays loaded** — VR init now runs |
| VR activation | Flatscreen only, no VR | **VR activates** — headset rendering works |
| Log file | Never created (DLL unloaded) | **Log file created** in `Portal 2\VR\portal2vr.log` |

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---|---|---|
| Game launches flat (no VR) | SteamVR not running or headset not detected | Start SteamVR first, check headset connection |
| Error popup on launch | VR_Init failed | Read the error message — usually SteamVR connection or GPU driver issue |
| Black screen in headset | Overlay texture race condition | Restart game with SteamVR already running |
| Stuttering / frame drops | Shader compilation stutter | Enable Vulkan shader pre-caching in Steam settings |
| DLL not loading | Steam updated Portal 2's d3d9.dll | Re-extract the mod files |
| Crash on launch | DXVK Vulkan device init failure | Update GPU drivers; verify Vulkan runtime is installed |
| Audio issues | Sound cache stale | Run `portal2_dlc3/UpdateSoundCache.cmd` in Portal 2 directory |
| Mod doesn't appear in SteamVR dashboard | Not registered | Run `VR\register_vr.bat` (requires SteamVR running) |

### Checking the Log File

If VR isn't working, always check `Portal 2\VR\portal2vr.log` first. The log shows:
- DLL attachment (`DLL_PROCESS_ATTACH`)
- Each engine DLL loaded (client, engine, materialsystem, server, vgui2)
- Interface acquisition
- VR_Init result (success or error description)
- Compositor, overlay, and render target setup
- When the VR system is fully initialized

---

## Building from Source

### Prerequisites
- Windows 10/11
- Visual Studio 2022 (v143 toolchain)
- Windows 10 SDK (10.0.26100.0+)
- Git (for submodule initialization)

### Build
```cmd
git clone https://github.com/jimgranitex-eng/Portal2VR.git
cd Portal2VR
git submodule init
git submodule update

msbuild l4d2vr.sln /p:Configuration=Release /p:Platform=x86 /t:Build
msbuild l4d2vr.sln /p:Configuration=Release /p:Platform=x64 /t:Build
```

Output: `Release\d3d9.dll` (x86) and `x64\Release\d3d9.dll` (x64)

Both DLLs are required — x86 goes to `Portal 2\` root, x64 goes to `Portal 2\bin\`.

---

## Repository

```
Portal2VR/
├── L4D2VR/                  Source code
│   ├── dllmain.cpp          Entry point, logging, version info
│   ├── game.cpp             Engine interface init, auto-exec settings
│   ├── vr.cpp               VR core: init, tracking, input, rendering
│   ├── vr.h                 VR class (full 6DoF + input)
│   ├── hooks.cpp            Source engine function hooks (MinHook)
│   ├── hooks.h              Hook declarations
│   ├── offsets.h            Signature-scanned function offsets
│   ├── config.txt           Live-reloadable VR settings
│   ├── manifest.vrmanifest  SteamVR app registration
│   ├── launch_vr.bat        One-click SteamVR + Portal 2 launcher
│   ├── register_vr.bat      One-time SteamVR registration
│   ├── version.rc           DLL metadata (v5.3.0.7)
│   ├── sdk/                 Source SDK headers
│   └── SteamVRActionManifest/  VR controller bindings
├── dxvk/                    D3D9 to Vulkan bridge (submodule)
├── thirdparty/              MinHook, OpenVR SDK
├── archive/v0.2.0/          Original release by Gistix (reference)
├── .github/workflows/       CI (x86 + x64 automated builds)
├── Portal2VR_v5.3.0.8.zip   Release package
├── l4d2vr.sln               Visual Studio solution
├── GUIDE.md                 Developer deep-dive
└── README.md                This file
```

---

## Release History

| Version | Date | Notes |
|---|---|---|
| **v5.3.0.8** | **2026-07-25** | **Current.** Fixed DllMain loader lock: removed `std::ofstream` from DllMain, preventing Windows from silently unloading the DLL. VR now activates. |
| v5.3.0.7 | 2026-07-24 | VR init reverted to v0.2.0 pattern: removed try/catch, Dashboard overlay, WindowCreatedHook, 30s timeout. Added file logging to VR\portal2vr.log. |
| v5.3.0.6 | 2026-07-24 | Auto-launch SteamVR + Portal 2, one-click batch launcher, simplified install |
| v5.3.0.5 | 2026-07-24 | Dual-arch CI, C++ casts cleanup, x64 link fix, DLL metadata |
| v5.3.0 | 2026-07-24 | Auto-detect VR, no launch options, auto-windowing, dashboard overlay |
| v5.2 | 2026-07-22 | Dead code cleanup, grabbable physics, perf optimization |
| v0.2.0 | pre-2026 | Original release by Gistix (archived) |

---

## Credits

**v5.2 – v5.3.0.8** — [jimgranitex-eng](https://github.com/jimgranitex-eng) — auto-VR, zero-config, left controller, code cleanup, perf, build fixes, dual-arch CI, loading screen skip, VR init fix, file logging, loader lock fix.

Original mod by [Gistix/portal2vr](https://github.com/Gistix/portal2vr), built on [sd805/l4d2vr](https://github.com/sd805/l4d2vr).

Uses [dxvk (vr-dx9-rel)](https://github.com/TheIronWolfModding/dxvk/tree/vr-dx9-rel), MinHook, OpenVR, source-sdk-2013.
