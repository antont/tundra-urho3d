@echo off
IF EXIST build-vs2010-x86\tundra-urho3d.sln del /Q build-vs2010-x86\tundra-urho3d.sln
cd tools\Windows\
call RunCMake "Visual Studio 10" %*
cd ..\..
pause
