@echo off
set LEONIDA_D3D12=vulkan
del /q F:\Work\Leonida\unsloth-tmp\boot_trace.json >nul 2>&1
"F:\Work\Leonida\runtime\build\gta6rt.exe" --run "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe" --game-dir "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK" --trace-d3d12 --trace F:\Work\Leonida\unsloth-tmp\boot_trace.json > F:\Work\Leonida\unsloth-tmp\boot_stdout.log 2>&1
echo GAME_EXIT=%ERRORLEVEL%
