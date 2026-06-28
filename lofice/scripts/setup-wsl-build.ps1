#Requires -Version 5.1
<#
.SYNOPSIS
  WSL에서 lofice lofice 빌드 트리 부트스트랩
.EXAMPLE
  .\lofice\scripts\setup-wsl-build.ps1
  .\lofice\scripts\setup-wsl-build.ps1 -InstallDeps
#>
param(
    [switch]$InstallDeps,
    [switch]$CheckOnly
)

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$RepoWsl = ($RepoRoot -replace '\\', '/')
if ($RepoWsl -match '^([A-Za-z]):(/.*)$') {
    $RepoWsl = '/mnt/' + $Matches[1].ToLower() + $Matches[2]
}

if ($CheckOnly) {
    wsl -e bash -lc "bash '$RepoWsl/lofice/scripts/verify-wsl-build-ready.sh'"
    exit $LASTEXITCODE
}

$envFlag = if ($InstallDeps) { "LOFICE_WSL_INSTALL_DEPS=1" } else { "" }
$cmd = @"
cd '$RepoWsl' && $envFlag bash lofice/scripts/bootstrap-wsl-build.sh
"@

Write-Host "WSL bootstrap: $RepoWsl"
if (-not $InstallDeps) {
    Write-Host "Tip: deps missing? re-run with -InstallDeps"
}

wsl -e bash -lc $cmd
exit $LASTEXITCODE
