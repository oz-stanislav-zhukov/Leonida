@echo off
cd /d F:\Work\Leonida
echo === BUILD RUNTIME (main + TLS fix) ===
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\build_runtime.ps1 -FetchSdl3 -Jobs 16 > unsloth-tmp\rebuild_mainfix.log 2>&1
if errorlevel 1 ( echo BUILD FAILED & exit /b 1 )
echo === RUN GAME ===
call unsloth-tmp\launch_gta6.bat > unsloth-tmp\run_mainfix.log 2>&1
C:\Python313\python.exe -I tools\boot_trace.py %TEMP%\leonida_boot.jsonl > unsloth-tmp\boot_triage_mainfix.txt 2>&1
echo === GPU SUITE (native + emulated) ===
call unsloth-tmp\run_gpu_suite.bat
echo ALL DONE
