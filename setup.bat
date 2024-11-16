@echo off
setlocal enabledelayedexpansion

:: Enable error checking
set "EXITCODE=0"

:: List all connected USB devices using usbipd list
echo Listing all USB devices...
for /f "tokens=*" %%A in ('usbipd list') do (
    echo %%A
)
if %errorlevel% neq 0 (
    echo Error: usbipd list command failed.
    exit /b %errorlevel%
)

:: Prompt user for the Bus ID
set /p busid="Enter the Bus ID of the device you want to forward to WSL (e.g., 4-2): "
if "%busid%"=="" (
    echo Error: Bus ID is required.
    exit /b 1
)
echo Selected Bus ID: %busid%

:: Bind the selected device
echo Binding the device with Bus ID %busid% to WSL...
usbipd bind --busid %busid%
if %errorlevel% neq 0 (
    echo Error: usbipd bind command failed.
    exit /b %errorlevel%
)

:: Attach the device to WSL
echo Attaching the device to WSL...
usbipd attach --wsl --busid %busid%
if %errorlevel% neq 0 (
    echo Error: usbipd attach command failed.
    exit /b %errorlevel%
)

:: Verify device is attached
echo Verify the device is attached...
usbipd list
if %errorlevel% neq 0 (
    echo Error: usbipd list command failed after attaching device.
    exit /b %errorlevel%
)
echo USB device successfully attached to WSL.

endlocal
pause
