# Portal 2 VR Mod

**v5.3.0** — *"One-Stop Shop"* — **July 24, 2026**

[![License](https://img.shields.io/badge/license-GPL--3.0-blue)](LICENSE)
[![Build](https://img.shields.io/badge/build-Release%20x86-brightgreen)](https://github.com/jimgranitex-eng/portal2vr)

Open-source VR mod for Portal 2. Motion controls, 6DoF, room-scale, grabbable objects.

**No launch options required.** Just extract into your Portal 2 folder and launch normally.
The mod auto-detects SteamVR, forces windowed mode, and applies optimal graphics settings.

**Latest release:** [`Portal2VR_v5.3.0.zip`](./Portal2VR_v5.3.0.zip) — July 24, 2026

---

## Quick Install

1. Download [`Portal2VR_v5.3.0.zip`](./Portal2VR_v5.3.0.zip)
2. Extract **all contents** into `steamapps\common\Portal 2` (merge folders)
3. Connect your headset and **start SteamVR**
4. **Launch Portal 2** normally from your Steam library

**That's it.** No launch options, no config tweaks, no SteamVR dashboard hunting.

---

## What's new in v5.3.0

| Feature | v5.2 (Previous) | v5.3.0 (Current) |
|---|---|---|
| Launch setup | Required `-insecure -window` + 8 cvars | **Zero config** — auto-detects everything |
| Window mode | Forced via `-window` flag | Auto-forced by DLL on launch |
| Graphics settings | Command-line cvars | Auto-applied 3s after engine starts |
| VR detection | Must be running before launch | **Auto-detects** SteamVR at runtime |
| No headset | Error popup, game may crash | **Graceful fallback** — runs flat, no popups |
| Left controller | Not tracked | **Full tracking** + rotation offset |
| Deployment | `bin\d3d9.dll` only | `bin\d3d9.dll` + **root d3d9.dll** |
| Build | Not CI-verified | **0 errors**, MSBuild Release x86 |

---

## Controls

| Action | Binding |
|---|---|
| Blue portal | Right trigger |
| Orange portal | Left trigger |
| Jump | A / Right touchpad |
| Crouch | B / Left touchpad |
| Use / Grab | Grip button |
| Reload | X / Y |
| Walk | Left stick |
| Turn | Right stick |
| Recenter | Left stick press |
| Flashlight | Stick button |
| Next / Prev item | D-pad up / down |
| Reset VR position | Stick press |

---

## Config

Edit `Portal 2\VR\config.txt` while the game is running — changes apply live.

| Setting | Default | Description |
|---|---|---|
| `AimMode` | 2 | 0=off, 1=crosshair, 2=laser sight |
| `SnapTurning` | false | Snap turn vs smooth |
| `SnapTurnAngle` | 45.0 | Degrees per snap |
| `LeftHanded` | false | Swap controller roles |
| `TurnSpeed` | 0.15 | Smooth turn speed |
| `VRScale` | 43.2 | World scale |
| `IPDScale` | 1.0 | IPD multiplier |
| `6DOF` | true | Positional tracking |
| `AntiAliasing` | 0 | MSAA level |
| `RenderWindow` | 0 | Desktop mirror window |
| `ViewmodelPosCustomOffset` | 0,0,0 | Viewmodel position |
| `ViewmodelAngCustomOffset` | 0,0,0 | Viewmodel angle |

---

## Building from source

Requirements: Visual Studio 2022 (v143), Windows 10 SDK (10.0.26100.0)

```cmd
msbuild l4d2vr.sln /p:Configuration=Release /p:Platform=x86 /t:Build
```

Output: `Release\d3d9.dll` (1,673,216 bytes)

---

## Repository

```
Portal2VR/
├── L4D2VR/                  Source code (v5.3.0)
│   ├── dllmain.cpp          Entry, auto-windowing, version
│   ├── game.cpp             Engine hooks, auto-exec settings
│   ├── vr.cpp               VR core, silent fallback
│   ├── vr.h                 VR class (full 6DoF + input)
│   ├── hooks.cpp            Source engine function hooks
│   ├── hooks.h              Hook declarations
│   ├── config.txt           Live-reloadable settings
│   ├── manifest.vrmanifest  SteamVR app registration
│   ├── sounds.h             Sound interface helper
│   ├── sdk/                 Source SDK headers
│   └── SteamVRActionManifest/  VR bindings
├── dxvk/                    D3D9→Vulkan bridge (submodule)
├── thirdparty/              MinHook, OpenVR
├── archive/v0.2.0/          Original release (reference)
├── .github/                 GitHub metadata
├── Portal2VR_v5.3.0.zip     Release package
├── l4d2vr.sln               Visual Studio solution
└── README.md                This file
```

---

## Release history

| Version | Date | Notes |
|---|---|---|
| **v5.3.0** | **2026-07-24** | **Current.** Auto-detect VR, no launch options needed, auto-windowing, auto-exec graphics, silent fallback, left controller tracking |
| v5.2 | 2026-07-22 | Dead code cleanup, grabbable physics, perf optimization, 0-error build |
| v0.2.0 | pre-2026 | Original release by Gistix (archived) |

---

## Credits

**v5.2 – v5.3.0** — [jimgranitex-eng](https://github.com/jimgranitex-eng) — auto-VR, zero-config, left controller, code cleanup, perf, build fixes.

Original mod by [Gistix/portal2vr](https://github.com/Gistix/portal2vr), built on [sd805/l4d2vr](https://github.com/sd805/l4d2vr).

Uses [dxvk (vr-dx9-rel)](https://github.com/TheIronWolfModding/dxvk/tree/vr-dx9-rel), MinHook, OpenVR, source-sdk-2013.
