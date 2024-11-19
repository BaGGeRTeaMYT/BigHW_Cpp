if (!(Test-Path ./build)) {
    mkdir build
}
Set-Location ./build ; cmake .. ; cmake --build . ; Set-Location ..