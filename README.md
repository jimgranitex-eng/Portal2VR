# Portal 2 VR Mod

**v5.3.0.6** — *"One-Stop Shop"* — **July 24, 2026 17:15 UTC**

[![License](https://img.shields.io/badge/license-GPL--3.0-blue)](LICENSE)
[![Build x86](https://img.shields.io/badge/build-Release%20x86-brightgreen)]()
[![Build x64](https://img.shields.io/badge/build-Release%20x64-brightgreen)]()

Open-source VR mod for Portal 2. Motion controls, 6DoF, room-scale, grabbable objects.

Works in **single player** and **co-op** — identical code path, zero extra config.

---

## Quick Install (2 minutes)

1. **Download** [`Portal2VR_v5.3.0.6.zip`](./Portal2VR_v5.3.0.6.zip)
2. **Extract** all contents into `steamapps\common\Portal 2` (merge folders)
3. **Launch Portal 2** normally from your Steam library — that's it

**No launch options, no config tweaks, no command-line flags.** The mod auto-detects SteamVR, forces windowed mode, and applies optimal graphics settings automatically. SteamVR starts on its own when you launch the game.

**Optional:** Run `launch_vr.bat` for one-click SteamVR + Portal 2 startup.

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
| Turn | Right stick (smooth or snap) |
| Recenter | Left stick press |
| Flashlight | Stick button |
| Next / Prev item | D-pad up / down |
| Reset VR position | Stick press |

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
| Overlay | None | **Dashboard tab** + loading screen overlay |
| CI | None | **Automated x86 + x64 builds** |
| Build | x86 only | **x86 + x64** (both 0 errors) |

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
msbuild l4d2vr.sln /p:Configuration=Release /p:Platform=x64 /t:Build
```

Output: `Release\d3d9.dll` (x86) and `x64\Release\d3d9.dll` (x64)

---

## Repository

```
Portal2VR/
├── L4D2VR/                  Source code (v5.3.0.5)
│   ├── dllmain.cpp          Entry, auto-windowing, version
│   ├── game.cpp             Engine hooks, auto-exec settings
│   ├── vr.cpp               VR core, silent fallback
│   ├── vr.h                 VR class (full 6DoF + input)
│   ├── hooks.cpp            Source engine function hooks
│   ├── hooks.h              Hook declarations
│   ├── offsets.h            Signature-scanned function offsets
│   ├── version.rc           DLL metadata (v5.3.0.6)
│   ├── config.txt           Live-reloadable settings
│   ├── manifest.vrmanifest  SteamVR app registration
│   ├── sounds.h             Sound interface helper
│   ├── sdk/                 Source SDK headers
│   └── SteamVRActionManifest/  VR bindings
├── dxvk/                    D3D9→Vulkan bridge (submodule)
├── thirdparty/              MinHook, OpenVR
├── archive/v0.2.0/          Original release (reference)
├── .github/workflows/       CI (x86 + x64 automated builds)
├── Portal2VR_v5.3.0.6.zip   Release package
├── l4d2vr.sln               Visual Studio solution
├── GUIDE.md                 Developer deep-dive
└── README.md                This file
```

---

## Release history

| Version | Date | Notes |
|---|---|---|
| **v5.3.0.6** | **2026-07-24 17:15** | **Current.** Auto-launch SteamVR + Portal 2, one-click batch launcher, simplified install |
| v5.3.0.5 | 2026-07-24 | Dual-arch CI, C++ casts cleanup, x64 link fix, DLL metadata |
| v5.3.0 | 2026-07-24 | Auto-detect VR, no launch options, auto-windowing, dashboard overlay, 0-error x86/x64 |
| v5.2 | 2026-07-22 | Dead code cleanup, grabbable physics, perf optimization, 0-error build |
| v0.2.0 | pre-2026 | Original release by Gistix (archived) |

---

## Credits

**v5.2 – v5.3.0.6** — [jimgranitex-eng](https://github.com/jimgranitex-eng) — auto-VR, zero-config, left controller, code cleanup, perf, build fixes, dual-arch CI.

Original mod by [Gistix/portal2vr](https://github.com/Gistix/portal2vr), built on [sd805/l4d2vr](https://github.com/sd805/l4d2vr).

Uses [dxvk (vr-dx9-rel)](https://github.com/TheIronWolfModding/dxvk/tree/vr-dx9-rel), MinHook, OpenVR, source-sdk-2013.
