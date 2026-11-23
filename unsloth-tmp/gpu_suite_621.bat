@echo off
rem Leonida GPU suite on driver 621.24, commit 5d74c2723 (for issue #166)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
cd /d F:\Work\Leonida
set LLVM_MC=F:\tools\llvm-mc\llvm-mc.exe
set PY=C:\Python313\python.exe
set OUT=F:\Leonida\gpu_suite_621

echo === PHASE 1: GAME RUN (10 min cap) ===
set LEONIDA_D3D12=vulkan
powershell -NoProfile -ExecutionPolicy Bypass -File unsloth-tmp\game_run_621.ps1
echo PHASE1_DONE

echo === PHASE 2: BUILD NATIVE ===
%PY% -I runtime\test\shaders\build.py %OUT%\native > unsloth-tmp\build_native_621.log 2>&1
echo BUILD_NATIVE_EXIT=%ERRORLEVEL%

echo === PHASE 3: CHECK NATIVE ===
%PY% -I runtime\test\shaders\check.py %OUT%\native runtime\build > unsloth-tmp\check_native_621.log 2>&1
echo CHECK_NATIVE_EXIT=%ERRORLEVEL%

echo === PHASE 4: GPU TESTS NATIVE ===
%PY% -I scripts\run_gpu_tests.py --build runtime\build > unsloth-tmp\gpunative_621.log 2>&1
echo GPUNATIVE_EXIT=%ERRORLEVEL%

echo === PHASE 5: BUILD EMULATED ===
set LEONIDA_W64_EMULATE=1
%PY% -I runtime\test\shaders\build.py %OUT%\emulated > unsloth-tmp\build_emu_621.log 2>&1
echo BUILD_EMU_EXIT=%ERRORLEVEL%

echo === PHASE 6: CHECK EMULATED ===
%PY% -I runtime\test\shaders\check.py %OUT%\emulated runtime\build > unsloth-tmp\check_emu_621.log 2>&1
echo CHECK_EMU_EXIT=%ERRORLEVEL%

echo === PHASE 7: GPU TESTS EMULATED ===
%PY% -I scripts\run_gpu_tests.py --build runtime\build > unsloth-tmp\gpuemu_621.log 2>&1
echo GPUEMU_EXIT=%ERRORLEVEL%

echo ALL_PHASES_DONE
