@echo off
set LLVM_MC=F:\tools\llvm-mc\llvm-mc.exe
cd /d F:\Work\Leonida
C:\Python313\python.exe -I runtime\test\shaders\check.py F:\Leonida-gpux F:\Work\Leonida\runtime\build
echo check exit: %errorlevel%
