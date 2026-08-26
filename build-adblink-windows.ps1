param()

$SourceDir = "C:\source\adblink"
$BuildDir = "C:\Users\jeff\build-adblink"

# Locate vcvars64.bat by detecting the VS install via vswhere, with a
# fallback to the Community Build Tools layout.
$vswhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
$vcvars = $null
if (Test-Path $vswhere) {
    $vswherePath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    if ($vswherePath -and (Test-Path "$vswherePath\VC\Auxiliary\Build\vcvars64.bat")) {
        $vcvars = "$vswherePath\VC\Auxiliary\Build\vcvars64.bat"
    }
}
if (-not $vcvars) {
    $fallback = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if (Test-Path $fallback) { $vcvars = $fallback }
}
if (-not $vcvars) {
    Write-Error "Could not locate vcvars64.bat (Visual Studio C++ toolchain)."
    exit 1
}

# Load Visual Studio environment so Ninja uses MSVC
cmd /c "`"$vcvars`" > nul 2>&1 && set" | ForEach-Object {
    if ($_ -match '^(\w+)=(.*)') {
        Set-Item -Path "env:$($matches[1])" -Value $matches[2]
    }
}

cmake -S $SourceDir -B $BuildDir -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build $BuildDir
ctest --test-dir $BuildDir --output-on-failure
