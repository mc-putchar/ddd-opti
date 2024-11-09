# Path to the backend directory
$backendDir = ".\backend"
# Relative path to the executable
$exePath = "$($backendDir)\ddd-backend.exe"
# Path to the make command (ensure it's available in the system's PATH or provide full path to the executable)
$makeCommand = "make"
$makeArgs = "-C $($backendDir)"
$portFilePath = ".\last_port.txt"

# Set-Location -Path $backendDir
# Run make to build the executable
$makeProcess = Start-Process $makeCommand -ArgumentList $makeArgs -PassThru -Wait
if ($makeProcess.ExitCode -eq 0) {
    Write-Host "'make' completed successfully."
} else {
    Write-Host "'make' failed with exit code: $($makeProcess.ExitCode)"
    exit 1
}
# Set-Location -Path (Get-Location)
$exeFullPath = Join-Path (Get-Location) $exePath

if (Test-Path $exeFullPath) {
    $portArgument = Get-Content -Path $portFilePath -Raw
    # Trim any extra whitespace or newlines
    $portArgument = $portArgument.Trim()
    # Start the process with the argument from the file
    $process = Start-Process $exeFullPath -ArgumentList $portArgument -PassThru

    # $process = Start-Process $exeFullPath -PassThru
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
