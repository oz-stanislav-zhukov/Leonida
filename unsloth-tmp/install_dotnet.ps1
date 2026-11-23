$ErrorActionPreference = "Stop"
$dir = Join-Path $env:LOCALAPPDATA "Microsoft\dotnet"
& (Join-Path $env:TEMP "dotnet-install.ps1") --channel 8.0 --install-dir $dir
if ($LASTEXITCODE -ne 0) { throw "dotnet-install failed ($LASTEXITCODE)" }
# add to user PATH if not already there
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
if (($userPath -split ";") -notcontains $dir) {
    [Environment]::SetEnvironmentVariable("Path", "$userPath;$dir", "User")
    Write-Host "added $dir to user PATH"
}
& (Join-Path $dir "dotnet.exe") --version
