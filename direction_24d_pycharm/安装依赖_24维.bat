@echo off
setlocal
chcp 65001 >nul
cd /d "%~dp0"
set "PY_CMD=python"
where py >nul 2>nul
if not errorlevel 1 set "PY_CMD=py -3"
%PY_CMD% --version >nul 2>nul
if errorlevel 1 goto no_python
%PY_CMD% -m pip install -r requirements.txt
if errorlevel 1 goto failed
echo Dependencies installed successfully.
pause
exit /b 0
:no_python
echo Python 3 was not found. Install Python and enable Add Python to PATH.
pause
exit /b 1
:failed
echo Dependency installation failed. Check the messages above.
pause
exit /b 1
