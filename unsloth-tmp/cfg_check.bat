@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
cd /d F:\Work\Leonida
cmake --build runtime/build --config RelWithDebInfo -n 2>&1
echo CFG_EXIT=%ERRORLEVEL%
