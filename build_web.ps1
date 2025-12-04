$emsdk = "C:\Users\ShadowEvil\source\repos\emsdk\emsdk"
& "$emsdk\emsdk_env.ps1"

# Add Ninja to PATH
$ninjaPath = "C:\Users\ShadowEvil\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe"
$env:PATH = "$ninjaPath;$env:PATH"

$buildDir = "web_build"
if (Test-Path $buildDir) { Remove-Item $buildDir -Recurse -Force }
mkdir $buildDir
cd $buildDir

# Use full paths to emscripten tools and cmake
$emcmake = "$emsdk\upstream\emscripten\emcmake.bat"
$cmake = "C:\Program Files\CMake\bin\cmake.exe"

& $emcmake $cmake .. -G Ninja

& $cmake --build .