@echo off
mkdir build
pushd build
call vcvarsall.bat x64
cl /std:c11 /EHsc /Zi /nologo /I ..\include ..\src\*.cpp ..\src\*.c /link /OUT:game.exe ..\lib\glfw3dll.lib	Dwmapi.lib || goto :error
.\game.exe
popd
goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
