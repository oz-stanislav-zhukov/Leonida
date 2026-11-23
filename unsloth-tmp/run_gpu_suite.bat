@echo off
set LLVM_MC=F:\tools\llvm-mc\llvm-mc.exe
cd /d F:\Work\Leonida
echo === NATIVE RUN === > unsloth-tmp\gpu_suite_native.log
C:\Python313\python.exe -I scripts\run_gpu_tests.py --build runtime\build >> unsloth-tmp\gpu_suite_native.log 2>&1
echo native exit: %errorlevel% >> unsloth-tmp\gpu_suite_native.log
echo === EMULATED RUN === > unsloth-tmp\gpu_suite_emulated.log
set LEONIDA_W64_EMULATE=1
C:\Python313\python.exe -I scripts\run_gpu_tests.py --build runtime\build >> unsloth-tmp\gpu_suite_emulated.log 2>&1
echo emulated exit: %errorlevel% >> unsloth-tmp\gpu_suite_emulated.log
