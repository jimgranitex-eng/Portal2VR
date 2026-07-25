@echo off
:: Portal 2 VR — One-Click Launch
:: Ensures SteamVR is running, then starts Portal 2

tasklist /FI "IMAGENAME eq vrstartup.exe" 2>NUL | find /I "vrstartup.exe" >NUL
if %ERRORLEVEL% NEQ 0 (
    echo Starting SteamVR...
    start "" "C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64\vrstartup.exe"
    timeout /t 5 /nobreak >NUL
)

echo Launching Portal 2 VR...
start steam://rungame/620
