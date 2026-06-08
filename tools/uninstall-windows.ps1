param(
    [string]$ObsPluginRoot = "C:\ProgramData\obs-studio\plugins"
)

$ErrorActionPreference = "Stop"

$TargetRoot = Join-Path $ObsPluginRoot "obs-glassveil"
if (-not (Test-Path -LiteralPath $TargetRoot)) {
    Write-Host "Glassveil is not installed at $TargetRoot"
    exit 0
}

Remove-Item -LiteralPath $TargetRoot -Recurse -Force
Write-Host "Removed Glassveil from $TargetRoot"
