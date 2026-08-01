@echo off
setlocal
chcp 65001 >nul
set "PYTHONUTF8=1"
cd /d "%~dp0"
set "PY_CMD=python"
where py >nul 2>nul
if not errorlevel 1 set "PY_CMD=py -3"
%PY_CMD% --version >nul 2>nul
if errorlevel 1 goto no_python
%PY_CMD% train_direction_24d.py --firmware-header "%~dp0..\..\Common\DualCore_Direction_Model_4ch_Weights.h"
if errorlevel 1 goto failed
echo.
echo Spatial-narrow14 shrinkage LDA training completed; the 24-element V5F header was replaced.
echo Clean and Build V5F in MounRiver, then flash V5F again.
pause
exit /b 0
:no_python
echo Python 3 was not found. Install Python and enable Add Python to PATH.
pause
exit /b 1
:failed
echo Spatial-narrow14 training failed. Check the messages above.
pause
exit /b 1
