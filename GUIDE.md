# Portal 2 VR Mod — Complete Developer Guide

**v5.3.0** — July 24, 2026

---

## 1. Overview

Portal 2 VR Mod is a Windows x86 DLL (`d3d9.dll`) that hooks into Portal 2 via DXVK
(D3D9→Vulkan bridge) and adds full SteamVR motion controls, 6DoF tracking, room-scale,
grabbable objects, and portal gun aiming.

The DLL replaces Portal 2's built-in `bin\d3d9.dll` and intercepts the D3D9 renderer
to render stereoscopic views to the VR headset while injecting VR input into the
engine's movement/aiming systems.

---

## 2. Repository Structure

```
Portal2VR/
├── L4D2VR/                       # Main source code
│   ├── dllmain.cpp               # Entry point, auto-windowing, version info
│   ├── game.cpp                  # Engine interface init, auto-exec settings
│   ├── game.h                    # Game class + Player struct
│   ├── vr.cpp                    # VR core: init, tracking, input, rendering
│   ├── vr.h                      # VR class (full 6DoF + input)
│   ├── hooks.cpp                 # Source engine function hooks (MinHook)
│   ├── hooks.h                   # Hook template + detour declarations
│   ├── offsets.h                 # Signature-scanned offsets for engine funcs
│   ├── sigscanner.h              # Pattern-scanning utility for offset discovery
│   ├── config.txt                # Live-reloadable user settings
│   ├── manifest.vrmanifest       # SteamVR application manifest
│   ├── portal2vr_capsule_main.png  # SteamVR store capsule image
│   ├── portal2vr_portrait_main.png # SteamVR store portrait image
│   ├── l4d2vr.vcxproj            # VS2022 project file
│   ├── l4d2vr.vcxproj.filters    # VS filter organization
│   ├── sdk/                      # Source SDK headers
│   │   ├── sdk.h                 # Main SDK include
│   │   ├── sounds.h              # IEngineSound interface
│   │   ├── vector.h              # Vector/QAngle math
│   │   ├── bitbuf.h/.cpp         # Bit buffer for usercmd serialization
│   │   ├── trace.h               # Ray trace structures
│   │   ├── texture.h             # ITexture interface
│   │   ├── material.h            # IMaterial interface
│   │   ├── platform.h            # Platform types
│   │   └── ...                   # Additional SDK headers
│   └── SteamVRActionManifest/    # SteamVR controller bindings
│       ├── action_manifest.json
│       ├── bindings_knuckles.json
│       ├── bindings_oculus_touch.json
│       └── bindings_vive_cosmos_controller.json
├── dxvk/                         # DXVK submodule (vr-dx9-rel fork)
│   ├── src/                      # D3D9/D3D10/D3D11/DXBC/DXSO/DXVK source
│   ├── include/                  # Vulkan/OpenVR/SPIRV headers
│   └── lib32/                    # Prebuilt x86 libraries
├── thirdparty/                   # Third-party dependencies
│   ├── minhook_src/              # MinHook source (API hooking library)
│   ├── minhook/lib/Release/      # Prebuilt MinHook x86 .lib
│   └── openvr/                   # SteamVR SDK headers + lib + dll
├── archive/
│   └── v0.2.0/                   # Original Gistix release (reference only)
├── .github/                      # GitHub metadata
│   ├── README.md
│   ├── FUNDING.yml
│   └── ISSUE_TEMPLATE/           # Bug report + feature request templates
├── .gitignore
├── l4d2vr.sln                    # Visual Studio 2022 solution
├── README.md                     # Project readme
├── GUIDE.md                      # This file
├── imgs/                         # Project images
└── Portal2VR_v5.3.0.zip          # Pre-built release package
```

---

## 3. How It Works

### 3.1 DLL Injection Chain
1. Portal 2 loads `d3d9.dll` (either from root or `bin\`)
2. `DllMain` → creates `InitL4D2VR` thread
3. `InitL4D2VR` prints version banner, creates `Game` instance
4. `Game` constructor spins until `client.dll`, `engine.dll`, `materialsystem.dll`, `server.dll`, `vgui2.dll` load
5. Interfaces are obtained via `CreateInterface`
6. `VR` is constructed — attempts SteamVR init (silent on failure)
7. `Hooks` are set up via MinHook
8. `AutoExecThread` is spawned — applies graphics cvars after 3s
9. `AutoWindowThread` is spawned — forces windowed 1280×720

### 3.2 VR Rendering Loop
- `D3D9VR_Present()` (from dxvk hook) → triggers `VR::Update()`
- `Update()` → `SubmitVRTextures()` → `VRCompositor::Submit()` for each eye
- Eye textures are created as named render targets via MaterialSystem
- Overlay handles menu/HUD rendering in VR space
- Config file is watched via `FindFirstChangeNotification` for live reload

### 3.3 Input Pipeline
- SteamVR action handles mapped to engine commands
- `CreateMove` hook injects VR headset yaw/pitch into view angles
- Controller positions tracked via `WaitGetPoses`
- Stick turning (smooth or snap) applied as rotation offset
- Portal gun aim uses laser sight via `SetControlPoint` on particle effect
- Grabbable objects: `ComputeError`/`UpdateObject`/`RotateObject` hooks

### 3.4 Signature Scanning
- All engine function offsets are stored in `offsets.h`
- At startup, `SigScanner` verifies each offset against pattern
- If offset changed (game update), scanner finds new address automatically
- Patterns are IDA-style byte sequences with `??` for wildcards

---

## 4. Build Instructions

### Prerequisites
- Windows 10/11
- Visual Studio 2022 (v143 toolchain)
- Windows 10 SDK (10.0.26100.0+)
- DirectX SDK (for `d3d9.def`)
- Git (for submodule initialization)

### Steps
```cmd
git clone https://github.com/jimgranitex-eng/Portal2VR.git
cd Portal2VR
git submodule init
git submodule update

msbuild l4d2vr.sln /p:Configuration=Release /p:Platform=x86 /t:Build
```

Output: `Release/d3d9.dll` (1,673,216 bytes, 0 errors)

### Configuration
- **Debug|Win32**: Debug build with console alloc, no optimizations
- **Release|Win32**: Production build with LTCG, optimized
- Both targets deploy to `C:\Program Files (x86)\Steam\steamapps\common\Portal 2\`
  automatically on successful build

### Key Build Settings (vcxproj)
| Setting | Value |
|---|---|
| Platform Toolset | v143 |
| C++ Standard | C++17 |
| Character Set | MultiByte |
| Linker Subsystem | Windows |
| DLL Name | `d3d9.dll` |
| DEP (NX) | On (via linker default) |
| ASLR | On (via linker default) |

---

## 5. Key Source Files

### dllmain.cpp
- Version defines `VER_PRODUCT`, `VER_VERSION`, `VER_DATE`
- `ForceWindowedMode()` — strips fullscreen via `SetWindowLongPtr`
- `AutoWindowThread` — retries 60×500ms to find game window
- Entry point creates Game, spawns auto-window thread

### game.cpp
- `Game::Game()` — Interface acquisition, object construction
- `AutoExecThread` — Applies 12 optimal graphics cvars after 3s delay
- `Game::errorMsg()` — MessageBox wrapper
- `Game::GetInterface()` — Source engine interface resolution

### vr.cpp (1,359 lines)
- `VR::VR()` — `VR_Init` with silent fallback, FOV calc, manifest install
- `VR::Update()` — Per-frame: texture submit, poses, input
- `VR::CreateVRTextures()` — Render target allocation for both eyes
- `VR::SubmitVRTextures()` — Submit to SteamVR compositor
- `VR::ProcessInput()` — Action→command translation with `sendOnTransition`
- `VR::ProcessMenuInput()` — Menu overlay laser pointer handling
- `VR::UpdateTracking()` — HMD position, controller position, viewmodel offsets
- `VR::ParseConfigFile()` — Settings parser with live-reload watcher

### hooks.cpp (905 lines)
- Constructor enables all hooks via MinHook
- `hkEmitSound` — VTable hook on `IEngineSound`
- `hkRenderView` — Triggers VR texture submission per frame
- `hkCreateMove` — Injects VR view angles
- `hkCalcViewModelView` — Positions viewmodel at controller
- `hkEyePosition` / `hkEyeAngles` — VR-aware eye position for portals
- `hkComputeError` / `hkUpdateObject` / `hkRotateObject` — Grabbable objects

### offsets.h (101 lines)
- 30+ signature-scanned offsets for `client.dll`, `engine.dll`, `server.dll`, `materialsystem.dll`
- Each offset auto-verifies at runtime via pattern scan

---

## 6. Configuration

### config.txt (live-reloadable)
| Key | Type | Default | Description |
|---|---|---|---|
| `AimMode` | int | 2 | 0=off, 1=crosshair, 2=laser |
| `SnapTurning` | bool | false | Snap vs smooth turn |
| `SnapTurnAngle` | float | 45.0 | Degrees per snap |
| `LeftHanded` | bool | false | Swap controllers |
| `TurnSpeed` | float | 0.15 | Smooth turn speed |
| `VRScale` | float | 43.2 | World scale |
| `IPDScale` | float | 1.0 | IPD multiplier |
| `6DOF` | bool | true | Positional tracking |
| `AntiAliasing` | int | 0 | MSAA samples |
| `RenderWindow` | int | 0 | Desktop mirror |
| `ViewmodelPosCustomOffset` | float XYZ | 0,0,0 | Custom offset |
| `ViewmodelAngCustomOffset` | float XYZ | 0,0,0 | Custom angle |

### Auto-applied cvars (hardcoded in game.cpp)
```
mat_motion_blur_percent_of_screen_max 0
mat_queue_mode 0
mat_vsync 0
mat_antialias 0
mat_grain_scale_override 0
mat_disable_bloom 1
fog_enable 0
r_drawmodelstatsoverlay 0
r_shadows 0
budget_show_history 0
cl_showfps 0
net_graph 0
```

---

## 7. Deployment

### Manual Install
1. Copy `d3d9.dll` to `Portal 2/bin/` (replaces original)
2. Copy `d3d9.dll` to `Portal 2/` root (backup load path)
3. Copy `VR/` folder to `Portal 2/`
4. Copy `dxvk.conf` to `Portal 2/`

### Auto-Deploy (Build)
- Post-build event in vcxproj copies both to `bin\` and root of Portal 2 install
- Portal 2 path: `C:\Program Files (x86)\Steam\steamapps\common\Portal 2`

### Release Packaging
```cmd
# Manual zip creation
Compress-Archive -Path bin\d3d9.dll,d3d9.dll,VR\*,dxvk.conf,README.md ^
  -DestinationPath Portal2VR_v5.3.0.zip
```

---

## 8. Dependency Graph

```
d3d9.dll (output)
├── dxvk/               (D3D9→Vulkan: full D3D9 implementation)
│   ├── d3d9_vr.cpp     (VR-specific DXVK extensions)
│   ├── dxbc/            (DXBC bytecode compiler)
│   ├── dxso/            (DX9 shader compiler)
│   ├── dxvk/            (Vulkan abstraction layer)
│   ├── spirv/           (SPIR-V module builder)
│   └── util/            (Config, logging, threading, hashing)
├── thirdparty/minhook/  (x86 API hooking)
├── thirdparty/openvr/   (SteamVR SDK)
├── L4D2VR/              (VR logic + engine integration)
│   ├── sdk/             (Source engine interface headers)
│   └── (source files)
└── SDK (Windows 10 + DirectX)
```

---

## 9. Coding Standards

- **Language**: C++17
- **Naming**: `m_` prefix for members, PascalCase for classes, camelCase for locals
- **Hooks**: `hk` prefix, detour functions prefixed `d`
- **Offsets**: Register in `offsets.h` with IDA-style signature
- **Memory**: No CRT memory allocation in hooks; use stack/static where possible
- **Thread safety**: VR::Update runs on render thread; config watcher on its own thread

---

## 10. Common Issues & Fixes

| Symptom | Likely Cause | Fix |
|---|---|---|
| DLL not loading | Steam updated Portal 2's d3d9.dll | Reinstall mod files |
| Crash on launch | DXVK Vulkan device init failure | Update GPU drivers; verify Vulkan runtime |
| No VR but game runs | SteamVR not running or no headset | Start SteamVR; check headset connection |
| Audio issues | Sound cache stale | Run `portal2_dlc3/UpdateSoundCache.cmd` |
| Black screen in HMD | Overlay texture init race | Restart game with SteamVR already running |
| Stuttering | Shader compilation stutter | Enable background Vulkan shader pre-caching in Steam |
| Wrong offsets | Game updated | Update signatures in `offsets.h`; rebuild |
