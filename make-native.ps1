$ErrorActionPreference = "Stop"

cmake --preset native-release
cmake --build build/release

if (Get-Command "llvm-strip" -ErrorAction SilentlyContinue) {
    llvm-strip build/release/decoder.exe
} else {
    Write-Warning "llvm-strip command not found. Skipping optimization step."
}

Write-Host "--- Compilation completed successfully! ---"
