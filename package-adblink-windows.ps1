param()

$BuildDir = "C:\Users\jeff\build-adblink"

# Load Visual Studio environment so Ninja uses MSVC
$vcvars = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmd /c "`"$vcvars`" > nul 2>&1 && set" | ForEach-Object {
    if ($_ -match '^(\w+)=(.*)') {
        Set-Item -Path "env:$($matches[1])" -Value $matches[2]
    }
}

cmake --build $BuildDir --target package-win
Get-ChildItem "$BuildDir\packages\"
