::@echo off
mkdir build
xcopy /Y lib\glfw3.dll build
pushd build
call vcvarsall.bat x64
cl /EHsc /Zi /nologo /I ..\include ..\src\*.cpp ..\src\*.c /link /OUT:game.exe ..\lib\glfw3dll.lib || goto :error
.\game.exe
popd
goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
