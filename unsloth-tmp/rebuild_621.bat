@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
cd /d F:\Work\Leonida
cmake --build runtime/build --config RelWithDebInfo -j 2>&1 | findstr /R "error LNK gta6rt XErrorReport"
echo BUILD_EXIT=%ERRORLEVEL%
