# Relative path to the executable
$exePath = ".\backend\ddd-backend.exe"
# Path to the backend directory
$backendDir = ".\backend"
# Path to the make command (ensure it's available in the system's PATH or provide full path to the executable)
$makeCommand = "make"

# Get the absolute path of the executable
$exeFullPath = Join-Path (Get-Location) $exePath
if (Test-Path $exeFullPath) {
    Write-Host "Executable found: $exeFullPath"
} else {
    Write-Host "Executable not found at path: $exeFullPath"
    Write-Host "Running 'make' to build the executable..."

    # Run make to build the executable
    Set-Location -Path $backendDir
    $makeProcess = Start-Process $makeCommand -PassThru -Wait
    if ($makeProcess.ExitCode -eq 0) {
        Write-Host "'make' completed successfully."
    } else {
        Write-Host "'make' failed with exit code: $($makeProcess.ExitCode)"
        exit 1
    }
    Set-Location -Path (Get-Location)
    $exeFullPath = Join-Path (Get-Location) $exePath
}

if (Test-Path $exeFullPath) {
    $process = Start-Process $exeFullPath -PassThru
    Write-Host "Controller running with PID: $($process.Id)"
    $process.WaitForExit()
    if ($process.ExitCode -eq 0) {
        Write-Host "Process exited successfully."
    } else {
        Write-Host "Process exited with code: $($process.ExitCode)"
    }
} else {
    Write-Host "Failed to find or build the executable."
    exit 1
}
