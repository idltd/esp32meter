@echo off
setlocal

set FQBN=esp32:esp32:esp32c3
set SKETCH=firmware\esp32meter
set OUTDIR=firmware\build

if not exist %OUTDIR% mkdir %OUTDIR%

echo [ESP32 Meter] Compiling for ESP32-C3...
arduino-cli compile --fqbn %FQBN% --output-dir %OUTDIR% %SKETCH%
if errorlevel 1 (
    echo.
    echo [ERROR] Compile failed. Check output above.
    exit /b 1
)

echo.
echo [ESP32 Meter] Build OK. Binary in %OUTDIR%\

if "%1"=="" (
    echo [ESP32 Meter] To flash:  build.bat COM5   ^(replace COM5 with your port^)
    goto :eof
)

echo [ESP32 Meter] Flashing to %1...
arduino-cli upload --fqbn %FQBN% -p %1 %SKETCH%
if errorlevel 1 (
    echo.
    echo [ERROR] Flash failed. Check the COM port and try again.
    exit /b 1
)

echo [ESP32 Meter] Done. Connect to WiFi "ESP32Meter_XXXX" and open http://192.168.4.1/

endlocal
