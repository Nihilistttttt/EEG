@echo off
echo ============================================
echo  EEG System - One-Click Start
echo ============================================
echo.

echo [1/3] Starting cpolar tunnel...
start /min "" "C:\Users\59169\Desktop\useful\cpolar_bin\cpolar.exe" start eeg-iotda
timeout /t 5 /nobreak >nul

echo [2/3] Starting Python Bridge...
start /min "" python "C:\Users\59169\Desktop\useful\eeg_web_server.py"
timeout /t 5 /nobreak >nul

echo [3/3] Opening browser...
start "" "http://localhost:8088"

echo.
echo ============================================
echo  All services started!
echo.
echo  Check cpolar URL at: http://localhost:9200
echo  If URL changed, update IoTDA forwarding rule.
echo ============================================
echo.
pause