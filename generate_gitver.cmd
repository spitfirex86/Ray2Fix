set TFILE="%~dp0\gitver.h"
for /f "tokens=*" %%s in ('git describe --always --tags --abbrev^=4 --dirty^=-d') do (set TDEF1=%%s)
for /f "tokens=*" %%s in ('git describe --always --tags --abbrev^=0') do (set TDEF2=%%s)

>%TFILE% (
echo #pragma once
echo.
echo #define C_GIT_VER "%TDEF1%"
echo #define C_GIT_TAG "%TDEF2%"
echo.
)
