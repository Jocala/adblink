param()

$SourceDir = "X:\adblink"
$BuildDir = "C:\Users\jeff\build-adblink"

cmake -S $SourceDir -B $BuildDir -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build $BuildDir --config Release
ctest --test-dir $BuildDir --output-on-failure -C Release
cpack --config "$BuildDir\CPackConfig.cmake" -B "$BuildDir\packages"
Get-ChildItem "$BuildDir\packages\"
