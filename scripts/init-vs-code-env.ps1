Param(
    [switch]$task
)

# --- EDIT PATH --- 
# Compiler 
$env:Path += ";C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64"

# CMake
$env:Path += ";C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"

# Ninja
$env:Path += ";C:\Program Files\Ninja"

# Windows SDK
$env:Path += ";C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64"

# Conan dependencies
$env:Path += ";C:\Users\sagec\.conan2\p\b\qtc2086cec4db27\p\bin;C:\Users\sagec\.conan2\p\b\doubl6d3cdeb4f20dd\p\bin;C:\Users\sagec\.conan2\p\b\harfb0b5b9428efb3a\p\bin;C:\Users\sagec\.conan2\p\b\freet39315e714104d\p\bin;C:\Users\sagec\.conan2\p\b\glibdca0f7a81998f\p\bin;C:\Users\sagec\.conan2\p\b\libffc60487ea0d166\p\bin;C:\Users\sagec\.conan2\p\b\pcre20d05a8a569af9\p\bin;C:\Users\sagec\.conan2\p\b\libge5af8fc841eb96\p\bin;C:\Users\sagec\.conan2\p\b\libic7384ba8c26702\p\bin;C:\Users\sagec\.conan2\p\b\libpn612d9a5448332\p\bin;C:\Users\sagec\.conan2\p\b\sqlit9854e33a66ccb\p\bin;C:\Users\sagec\.conan2\p\b\libpq814729a6dd150\p\bin;C:\Users\sagec\.conan2\p\b\brotlb2589c3c65d0c\p\bin;C:\Users\sagec\.conan2\p\b\md4cca5979e76084e\p\bin;C:\Users\sagec\.conan2\p\b\spdlo6b4feb7970c7f\p\bin;C:\Users\sagec\.conan2\p\b\fmt3c1b51399bcd5\p\bin;C:\Users\sagec\.conan2\p\b\gtest1b22cfc141f98\p\bin;C:\Users\sagec\.conan2\p\b\boost26f79f0f6e192\p\bin;C:\Users\sagec\.conan2\p\b\bzip2633f6941e5533\p\bin;C:\Users\sagec\.conan2\p\b\opens00f639262623a\p\bin;C:\Users\sagec\.conan2\p\b\zlib990f81241e3d1\p\bin"


# --- CONAN STUFF ---
$env:QT_PLUGIN_PATH="C:\Users\sagec\.conan2\p\b\qtc2086cec4db27\p\res\archdatadir\plugins"
$env:QT_HOST_PATH="C:\Users\sagec\.conan2\p\b\qtc2086cec4db27\p"
$env:OPENSSL_MODULES="C:\Users\sagec\.conan2\p\b\opens00f639262623a\p\lib\ossl-modules"


if (-not $task) {
    Write-Host "Interactive session."
} else {
    # Task execution, exit when done
    Write-Host "Task execution. Exiting."
    exit
}