param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

# Setup logging
$logFile = Join-Path $PSScriptRoot "build.log"
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

# Function to write to both console and log
function Write-BuildLog {
    param(
        [string]$Message,
        [string]$ForegroundColor = "White"
    )
    $logMessage = "[$timestamp] $Message"
    Write-Host $Message -ForegroundColor $ForegroundColor
    Add-Content -Path $logFile -Value $logMessage
}

# Clear previous log
if (Test-Path $logFile) {
    Remove-Item $logFile
}

Write-BuildLog "========================================" -ForegroundColor Cyan
Write-BuildLog "Web Build Log - $timestamp" -ForegroundColor Cyan
Write-BuildLog "========================================" -ForegroundColor Cyan
Write-BuildLog ""

$emsdk = "C:\Users\ShadowEvil\source\repos\emsdk\emsdk"
& "$emsdk\emsdk_env.ps1" 2>&1 | Tee-Object -FilePath $logFile -Append | Out-Null

# Add Ninja to PATH
$ninjaPath = "C:\Users\ShadowEvil\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe"
$env:PATH = "$ninjaPath;$env:PATH"

Write-Host ""
Write-BuildLog "========================================" -ForegroundColor Cyan
Write-BuildLog "  Building Web ($Configuration)" -ForegroundColor Green
Write-BuildLog "========================================" -ForegroundColor Cyan
Write-Host ""

# Build directly in output directory
$outputDir = "Build\Web\$Configuration"

Write-BuildLog "Preparing build directory: $outputDir" -ForegroundColor Yellow
if (Test-Path $outputDir) {
    Remove-Item "$outputDir\*" -Recurse -Force
}
mkdir $outputDir -Force | Out-Null

# Use full paths to emscripten tools and cmake
$emcmake = "$emsdk\upstream\emscripten\emcmake.bat"
$cmake = "C:\Program Files\CMake\bin\cmake.exe"

Write-BuildLog "Running CMake configuration..." -ForegroundColor Yellow
# Configure with CMake (from output directory, referencing web_build CMakeLists.txt)
cd $outputDir
& $emcmake $cmake "$PSScriptRoot\web_build" -G Ninja "-DCMAKE_BUILD_TYPE=$Configuration" 2>&1 | Tee-Object -FilePath $logFile -Append

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-BuildLog "CMake configuration failed!" -ForegroundColor Red
    Write-BuildLog "Check $logFile for details" -ForegroundColor Yellow
    cd $PSScriptRoot
    exit 1
}

Write-BuildLog "Building project..." -ForegroundColor Yellow
# Build
& $cmake --build . 2>&1 | Tee-Object -FilePath $logFile -Append

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-BuildLog "Build failed!" -ForegroundColor Red
    Write-BuildLog "Check $logFile for details" -ForegroundColor Yellow
    cd $PSScriptRoot
    exit 1
}

# Return to root directory first
cd $PSScriptRoot

# Copy web assets to build directory
Write-BuildLog "Copying web assets..." -ForegroundColor Yellow
$assetsSource = Join-Path $PSScriptRoot "web_build\Assets"
$assetsDest = Join-Path $PSScriptRoot "$outputDir\Assets"

if (Test-Path $assetsSource) {
    if (Test-Path $assetsDest) {
        Remove-Item $assetsDest -Recurse -Force
    }
    Copy-Item -Path $assetsSource -Destination $assetsDest -Recurse -Force
    Write-BuildLog "Assets copied successfully" -ForegroundColor Green
} else {
    Write-BuildLog "Warning: Assets directory not found at $assetsSource" -ForegroundColor Yellow
}

Write-Host ""
Write-BuildLog "========================================" -ForegroundColor Cyan
Write-BuildLog "  Build Completed Successfully!" -ForegroundColor Green
Write-BuildLog "========================================" -ForegroundColor Cyan
Write-Host ""
Write-BuildLog "  Configuration: $Configuration" -ForegroundColor White
Write-BuildLog "  Output: $outputDir" -ForegroundColor Cyan
Write-BuildLog "  Log file: $logFile" -ForegroundColor DarkGray
Write-Host ""
Write-BuildLog "To serve the build, run:" -ForegroundColor Yellow
Write-Host "  .\serve.ps1 -Configuration $Configuration" -ForegroundColor White
Write-Host ""