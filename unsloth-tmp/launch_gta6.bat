@echo off
set LEONIDA_D3D12=vulkan
del /q "%TEMP%\leonida_boot.jsonl" "%TEMP%\leonida_d3d12.log" 2>nul
F:\Work\Leonida\runtime\build\gta6rt.exe --run "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe" --game-dir "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK" --trace "%TEMP%\leonida_boot.jsonl" --trace-d3d12 2> "%TEMP%\leonida_d3d12.log"
echo exit code: %errorlevel%
