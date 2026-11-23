@echo off
set LEONIDA_D3D12=vulkan
del /q "%TEMP%\leonida_boot.jsonl" 2>nul
F:\Leonida-pr8379\runtime\build\gta6rt.exe --run "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe" --game-dir "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK" --trace "%TEMP%\leonida_boot.jsonl" --trace-d3d12
echo exit code: %errorlevel%
