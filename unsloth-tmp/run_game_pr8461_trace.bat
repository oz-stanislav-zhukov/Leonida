@echo off
set LEONIDA_D3D12=vulkan
"F:\Leonida-pr8461\runtime\build\gta6rt.exe" --run "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe" --game-dir "F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK" --trace-d3d12 --trace F:\Work\Leonida\unsloth-tmp\pr8461_trace.json > F:\Work\Leonida\unsloth-tmp\pr8461_stdout.log 2>&1
echo GAME_EXIT=%ERRORLEVEL%
