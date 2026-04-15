param(
    [Parameter(Mandatory = $true)]
    [ValidateSet(
        "build-win-client",
        "build-win-server",
        "build-win-all",
        "run-win-client",
        "run-win-server",
        "run-win-fullstack"
    )]
    [string]$Task,

    [ValidateSet("Debug", "Release")]
    [string]$BuildType = "Release"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

function Write-Stage([string]$Message) {
    Write-Host "==> $Message"
}

function Assert-LastExitCode([string]$Operation) {
    if ($LASTEXITCODE -ne 0) {
        throw "$Operation failed with exit code $LASTEXITCODE"
    }
}

function Get-RepoRoot() {
    return (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
}

function Get-MirrorRoot() {
    if ($env:SAGESTORE_WINDOWS_MIRROR_ROOT) {
        return $env:SAGESTORE_WINDOWS_MIRROR_ROOT
    }

    return (Join-Path $env:LOCALAPPDATA "Temp\SageStore-win-src")
}

function Get-BuildDir([string]$MirrorRoot) {
    return (Join-Path $MirrorRoot "build-win")
}

function Sync-RepoToMirror([string]$RepoRoot, [string]$MirrorRoot) {
    Write-Stage "Sync repo to Windows mirror"
    New-Item -ItemType Directory -Force -Path $MirrorRoot | Out-Null

    $null = robocopy $RepoRoot $MirrorRoot /MIR /R:1 /W:1 /NFL /NDL /NJH /NJS /NP `
        /XD ".git" "build" "build-win" "logs" ".codex"

    if ($LASTEXITCODE -gt 7) {
        throw "robocopy failed with exit code $LASTEXITCODE"
    }
}

function Get-QtRoot() {
    $candidates = Get-ChildItem "C:\Qt" -Directory -ErrorAction SilentlyContinue |
        ForEach-Object {
            $qtRoot = Join-Path $_.FullName "mingw_64"
            $qtConfig = Join-Path $qtRoot "lib\cmake\Qt6\Qt6Config.cmake"
            if ((Test-Path (Join-Path $qtRoot "bin\Qt6Core.dll")) -and (Test-Path $qtConfig)) {
                [PSCustomObject]@{
                    Root = $qtRoot
                    VersionName = $_.Name
                    IsVersionDir = ($_.Name -match '^\d+(\.\d+)*$')
                }
            }
        } |
        Sort-Object `
            @{ Expression = { $_.IsVersionDir }; Descending = $true }, `
            @{ Expression = { if ($_.IsVersionDir) { [version]$_.VersionName } else { [version]"0.0" } }; Descending = $true }, `
            @{ Expression = { $_.Root }; Descending = $true }

    if (-not $candidates) {
        throw "Unable to find a Qt6 mingw_64 SDK with Qt6Config.cmake under C:\Qt"
    }

    return $candidates[0].Root
}

function Get-QtCMakeDir([string]$QtRoot) {
    $qtCMakeDir = Join-Path $QtRoot "lib\cmake\Qt6"
    if (-not (Test-Path (Join-Path $qtCMakeDir "Qt6Config.cmake"))) {
        throw "Qt6Config.cmake not found under $qtCMakeDir"
    }

    return $qtCMakeDir
}

function Get-MingwBinDir() {
    $candidates = Get-ChildItem "C:\Qt\Tools" -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -like "mingw*" } |
        ForEach-Object {
            $binDir = Join-Path $_.FullName "bin"
            if (
                (Test-Path (Join-Path $binDir "g++.exe")) -and
                (Test-Path (Join-Path $binDir "mingw32-make.exe"))
            ) {
                $sortKey = 0
                if ($_.Name -match '^mingw(\d+)') {
                    $sortKey = [int]$Matches[1]
                }

                [PSCustomObject]@{
                    BinDir = $binDir
                    SortKey = $sortKey
                }
            }
        } |
        Where-Object {
            $_ -ne $null
        } |
        Sort-Object `
            @{ Expression = { $_.SortKey }; Descending = $true }, `
            @{ Expression = { $_.BinDir }; Descending = $true }

    if (-not $candidates) {
        throw "Unable to find a MinGW toolchain under C:\Qt\Tools"
    }

    return $candidates[0].BinDir
}

function Get-GccMajorVersion([string]$MingwBinDir) {
    $gccVersion = (& (Join-Path $MingwBinDir "g++.exe") -dumpfullversion -dumpversion | Select-Object -First 1)
    Assert-LastExitCode "Detect MinGW g++ version"
    if (-not $gccVersion) {
        throw "Unable to detect MinGW g++ version"
    }

    return (($gccVersion -split "\.")[0])
}

function Initialize-WindowsBuildEnvironment([string]$QtRoot, [string]$MingwBinDir) {
    $qtBinDir = Join-Path $QtRoot "bin"
    $env:PATH = "{0};{1};{2}" -f $MingwBinDir, $qtBinDir, $env:PATH
    $env:QT_DIR = $QtRoot
    $env:QT6_DIR = Get-QtCMakeDir $QtRoot
    $env:QT_PLUGIN_PATH = Join-Path $QtRoot "plugins"
    $env:QT_QPA_PLATFORM_PLUGIN_PATH = Join-Path $QtRoot "plugins\platforms"
}

function Invoke-ConanInstall([string]$MirrorRoot, [string]$BuildType, [string]$GccMajorVersion) {
    Write-Stage "Install Conan dependencies (${BuildType})"
    $previousPolicyVersion = [Environment]::GetEnvironmentVariable("CMAKE_POLICY_VERSION_MINIMUM", "Process")
    Push-Location $MirrorRoot
    try {
        [Environment]::SetEnvironmentVariable("CMAKE_POLICY_VERSION_MINIMUM", "3.5", "Process")
        try {
            & conan install . --output-folder=build-win --build=missing `
                -s:h os=Windows `
                -s:h arch=x86_64 `
                -s:h compiler=gcc `
                -s:h compiler.version=$GccMajorVersion `
                -s:h compiler.libcxx=libstdc++11 `
                -s:h build_type=$BuildType `
                -s:b os=Windows `
                -s:b arch=x86_64 `
                -s:b compiler=gcc `
                -s:b compiler.version=$GccMajorVersion `
                -s:b compiler.libcxx=libstdc++11 `
                -s:b build_type=$BuildType | Out-Host
            Assert-LastExitCode "Conan install"
        } finally {
            if ([string]::IsNullOrEmpty($previousPolicyVersion)) {
                Remove-Item Env:CMAKE_POLICY_VERSION_MINIMUM -ErrorAction SilentlyContinue
            } else {
                [Environment]::SetEnvironmentVariable("CMAKE_POLICY_VERSION_MINIMUM", $previousPolicyVersion, "Process")
            }
        }
    } finally {
        Pop-Location
    }
}

function Configure-CMake(
    [string]$MirrorRoot,
    [string]$BuildType,
    [string]$QtRoot,
    [string]$MingwBinDir,
    [bool]$BuildClient,
    [bool]$BuildServer
) {
    Write-Stage "Configure CMake (${BuildType})"
    $buildDir = Get-BuildDir $MirrorRoot
    Remove-Item -Recurse -Force (Join-Path $buildDir "CMakeFiles") -ErrorAction SilentlyContinue
    Remove-Item -Force (Join-Path $buildDir "CMakeCache.txt") -ErrorAction SilentlyContinue

    $toolchainFile = (Get-Item (Join-Path $buildDir "conan_toolchain.cmake")).FullName
    $makeProgram = (Join-Path $MingwBinDir "mingw32-make.exe")
    $qtCMakeDir = Get-QtCMakeDir $QtRoot

    $cmakeArgs = @(
        "-S", $MirrorRoot,
        "-B", $buildDir,
        "-G", "MinGW Makefiles",
        "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile",
        "-DCMAKE_MAKE_PROGRAM=$makeProgram",
        "-DCMAKE_POLICY_DEFAULT_CMP0091=NEW",
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_PREFIX_PATH=$QtRoot",
        "-DQt6_DIR=$qtCMakeDir",
        ("-DBUILD_CLIENT=" + $(if ($BuildClient) { "ON" } else { "OFF" })),
        ("-DBUILD_SERVER=" + $(if ($BuildServer) { "ON" } else { "OFF" })),
        "-DBUILD_TESTS=OFF"
    )

    & cmake @cmakeArgs | Out-Host
    Assert-LastExitCode "Configure CMake"
}

function Invoke-CMakeBuild([string]$MirrorRoot) {
    Write-Stage "Build targets"
    & cmake --build (Get-BuildDir $MirrorRoot) --parallel | Out-Host
    Assert-LastExitCode "Build targets"
}

function Ensure-Build([string]$Mode, [string]$BuildType) {
    $repoRoot = Get-RepoRoot
    $mirrorRoot = Get-MirrorRoot
    $qtRoot = Get-QtRoot
    $mingwBinDir = Get-MingwBinDir
    $gccMajorVersion = Get-GccMajorVersion $mingwBinDir

    Sync-RepoToMirror $repoRoot $mirrorRoot
    Initialize-WindowsBuildEnvironment $qtRoot $mingwBinDir
    Invoke-ConanInstall $mirrorRoot $BuildType $gccMajorVersion

    switch ($Mode) {
        "client" {
            Configure-CMake $mirrorRoot $BuildType $qtRoot $mingwBinDir $true $false
        }
        "server" {
            Configure-CMake $mirrorRoot $BuildType $qtRoot $mingwBinDir $false $true
        }
        "all" {
            Configure-CMake $mirrorRoot $BuildType $qtRoot $mingwBinDir $true $true
        }
        default {
            throw "Unsupported build mode '$Mode'"
        }
    }

    Invoke-CMakeBuild $mirrorRoot
    return $mirrorRoot
}

function Get-ClientExe([string]$MirrorRoot) {
    return (Join-Path (Get-BuildDir $MirrorRoot) "_client\SageStoreClient.exe")
}

function Get-ServerExe([string]$MirrorRoot) {
    return (Join-Path (Get-BuildDir $MirrorRoot) "_server\SageStoreServer.exe")
}

function Start-NativeProcess([string]$ExecutablePath) {
    if (-not (Test-Path $ExecutablePath)) {
        throw "Executable not found: $ExecutablePath"
    }

    $process = Start-Process -FilePath $ExecutablePath `
        -WorkingDirectory (Split-Path -Parent $ExecutablePath) `
        -PassThru

    Write-Host ("[sagestore-windows] started {0} (pid={1})" -f $ExecutablePath, $process.Id)
    return $process
}

function Test-TcpPortOpen(
    [string]$Address = "127.0.0.1",
    [int]$Port = 8001,
    [int]$TimeoutMs = 200
) {
    $client = New-Object System.Net.Sockets.TcpClient
    try {
        $asyncResult = $client.BeginConnect($Address, $Port, $null, $null)
        if (-not $asyncResult.AsyncWaitHandle.WaitOne($TimeoutMs)) {
            return $false
        }

        $client.EndConnect($asyncResult)
        return $true
    } catch {
        return $false
    } finally {
        $client.Dispose()
    }
}

function Wait-TcpPort(
    [string]$Address = "127.0.0.1",
    [int]$Port = 8001,
    [int]$TimeoutSeconds = 5
) {
    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if (Test-TcpPortOpen -Address $Address -Port $Port) {
            return
        }

        Start-Sleep -Milliseconds 50
    }

    throw "Timed out waiting for ${Address}:$Port"
}

switch ($Task) {
    "build-win-client" {
        $mirrorRoot = Ensure-Build "client" $BuildType
        Write-Host ("[sagestore-windows] client ready: {0}" -f (Get-ClientExe $mirrorRoot))
    }
    "build-win-server" {
        $mirrorRoot = Ensure-Build "server" $BuildType
        Write-Host ("[sagestore-windows] server ready: {0}" -f (Get-ServerExe $mirrorRoot))
    }
    "build-win-all" {
        $mirrorRoot = Ensure-Build "all" $BuildType
        Write-Host ("[sagestore-windows] fullstack ready in {0}" -f (Get-BuildDir $mirrorRoot))
    }
    "run-win-client" {
        $mirrorRoot = Ensure-Build "client" $BuildType
        if (-not (Test-TcpPortOpen)) {
            Write-Host "[sagestore-windows] warning: server is not listening on 127.0.0.1:8001"
        }
        Start-NativeProcess (Get-ClientExe $mirrorRoot) | Out-Null
    }
    "run-win-server" {
        $mirrorRoot = Ensure-Build "server" $BuildType
        if (Test-TcpPortOpen) {
            Write-Host "[sagestore-windows] server already listening on 127.0.0.1:8001, skipping launch"
        } else {
            Start-NativeProcess (Get-ServerExe $mirrorRoot) | Out-Null
        }
    }
    "run-win-fullstack" {
        $mirrorRoot = Ensure-Build "all" $BuildType
        if (Test-TcpPortOpen) {
            Write-Host "[sagestore-windows] server already listening on 127.0.0.1:8001"
        } else {
            Start-NativeProcess (Get-ServerExe $mirrorRoot) | Out-Null
            Wait-TcpPort
        }
        Start-NativeProcess (Get-ClientExe $mirrorRoot) | Out-Null
    }
}
