# Portal 2 VR — Development Log

## v5.3.0.8 — Loader Lock Fix (2026-07-25)

### Bug: VR never activates, no log file created
- Game loads in flatscreen, renders through DXVK but no VR headset output
- No `VR\portal2vr.log` file created anywhere on disk
- DLL has all correct code (strings verified in binary)
- Game runs normally using system d3d9.dll instead of ours

### Root Cause: `std::ofstream` inside `DllMain`
```
DllMain runs under the Windows LOADER LOCK.
Calling CRT functions (std::ofstream, sprintf_s, std::cout) inside DllMain
causes a DEADLOCK. Windows silently UNLOADS the DLL.
The game falls back to system d3d9.dll → flatscreen rendering, no VR.
```

### Fix
- Removed ALL `VRLog()` calls from `DllMain` in `dllmain.cpp`
- DllMain now only does: `DisableThreadLibraryCalls` + `CreateThread`
- VRLog is only called from `InitL4D2VR` thread (after loader lock released)
- Same fix needed in `game.cpp` and `vr.cpp` — VRLog must never be called
  from any code path that executes during DLL initialization

### Files Changed
- `L4D2VR/dllmain.cpp` — removed `VRLog("DLL_PROCESS_ATTACH")` from DllMain

### Key Takeaway
**NEVER use CRT functions inside `DllMain` on Windows.** The loader lock
prevents most API and CRT calls. Only use Win32 API calls that are
explicitly safe under loader lock: `DisableThreadLibraryCalls`,
`CreateThread`, `GetProcAddress`, `GetModuleHandle`. Move all other
logic to a thread created from DllMain.

Reference: https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices

---

## v5.3.0.7 — VR Init Reverted to v0.2.0 Pattern (2026-07-24)

### Bug: VR not activating despite correct code
- Compared working v0.2.0 with current v5.3.0.7
- Found 5 differences causing VR to not activate

### Fixes Applied
1. **Removed `try/catch(...)` around VR_Init** — was silently swallowing errors
2. **Replaced `CreateDashboardOverlay` with `CreateOverlay("MenuOverlayKey")`** — dashboard overlay behaves differently for texture submission
3. **Removed g_D3DVR9 30-second timeout** — original waited forever, timeout could abort before DXVK bridge ready
4. **Removed `WindowCreatedHook` + `SetWinEventHook`** — forcing windowed mode interfered with VR runtime
5. **Removed `m_Input`/`m_Overlay` null early-returns** — causing premature exits original never had
6. **Removed loading screen overlay** creation

### Additional Changes
- Added `Game::errorMsg()` (MessageBox) on VR_Init failure — visible error
- Added file-based `VRLog()` function to all 3 source files
- Removed critical interface null-check in `game.cpp` that could skip VR creation
- Replaced `GetCurrentDirectory()` with `GetModuleFileName(NULL)` for log path

### Files Changed
- `L4D2VR/vr.cpp` — VR constructor rewritten
- `L4D2VR/dllmain.cpp` — stripped to match v0.2.0 pattern
- `L4D2VR/game.cpp` — removed critical null-check, added logging

---

## v5.3.0.7 — Documentation Update (2026-07-24)

### Changes
- Complete README rewrite with user guide, install steps, controls, config
- Troubleshooting table with 8 common issues
- Log file explanation
- GUIDE.md updated: removed deleted code references (WindowCreatedHook, ForceWindowedMode, AutoWindowThread)

---

## v5.3.0.7 — Code Quality (2026-07-24)

### Changes
- All 36 C-style casts replaced with C++ casts across 4 files
- `dllmain.cpp`: `(LPTHREAD_START_ROUTINE)` → `reinterpret_cast<LPTHREAD_START_ROUTINE>`
- `hooks.cpp`: `(IClientMode*)` → `reinterpret_cast<IClientMode*>`, etc.
- `game.cpp`: `(IClientMode*)` → `reinterpret_cast<IClientMode*>`
- `vr.cpp`: `(float*)` → `reinterpret_cast<float*>`, etc.

---

## v5.3.0.7 — Skip Loading Screen Feature (2026-07-24)

### Changes
- Added `SkipLoadingScreen` config option (default: true)
- Detects loading screen overlay by name "Portal 2 VR Loading"
- Auto-hides loading overlay when detected
- Auto-plays intro videos via engine commands
- Config is live-reloadable (edit `VR\config.txt` while running)

---

## v5.3.0.7 — Version Bump (2026-07-24)

### Files Updated
- `dllmain.cpp`: VER_VERSION, VER_DATE
- `version.rc`: FileVersion, ProductVersion
- `config.txt`: header comment
- `manifest.vrmanifest`: version field

---

## Architecture Notes

### DLL Loading Chain
```
Portal 2 (portal2.exe, 32-bit)
  → Windows loads d3d9.dll from game root directory
  → DllMain fires (UNDER LOADER LOCK)
  → DisableThreadLibraryCalls(hModule)
  → CreateThread(InitL4D2VR)  [thread waits for loader lock to release]
  → Thread runs:
      → VRLog("InitL4D2VR thread started")
      → new Game()  [spins waiting for engine DLLs]
      → new VR()    [VR_Init, compositor, overlay]
      → new Hooks() [MinHook detours]
      → AutoExecThread [applies graphics settings]
```

### VR Init Path
```
VR::VR()
  → vr::VR_Init(&error, VRApplication_Scene)
  → vr::VRCompositor()
  → vr::VRInput()
  → OpenVRInternal_ModuleContext().VRSystem()
  → GetRecommendedRenderTargetSize()
  → GetProjectionRaw() for both eyes
  → InstallApplicationManifest()
  → SetActionManifest()
  → WaitForConfigUpdate thread
  → Wait for g_D3DVR9 (DXVK bridge)
  → GetBackBufferData()
  → VROverlay() → CreateOverlay("MenuOverlayKey")
  → UpdatePosesAndActions()
  → m_IsInitialized = true
```

### Debug Logging
- `VRLog()` writes to `Portal 2\VR\portal2vr.log`
- Uses `GetModuleFileName(NULL)` to find game exe directory
- Append mode — each launch adds to the log
- Prefixed with `[Game]` in game.cpp for identification
- Timestamp format: `[HH:MM:SS.mmm] message`
