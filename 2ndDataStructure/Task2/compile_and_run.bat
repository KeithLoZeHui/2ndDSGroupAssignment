@echo off
echo Compiling KeithLoZeHuiTask2.cpp...
g++ -o KeithLoZeHuiTask2 KeithLoZeHuiTask2.cpp
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo Compilation successful!
echo.
echo Running KeithLoZeHuiTask2...
KeithLoZeHuiTask2.exe
pause
