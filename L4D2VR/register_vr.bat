@echo off
:: Portal 2 VR — SteamVR Registration
:: Run once if mod doesn't appear in SteamVR dashboard
:: Requires SteamVR to be running

set MANIFEST=%~dp0VR\manifest.vrmanifest

if not exist "%MANIFEST%" (
    echo ERROR: manifest.vrmanifest not found at %MANIFEST%
    echo Make sure you extracted the ZIP into your Portal 2 folder.
    pause
    exit /b 1
)

echo Registering Portal 2 VR with SteamVR...
echo Manifest: %MANIFEST%
echo.
echo If SteamVR is running, Portal 2 VR should now appear in your dashboard.
echo If not, try restarting SteamVR.
pause
