param(
    [string]$PackageRoot = "",
    [string]$ObsPluginRoot = "C:\ProgramData\obs-studio\plugins"
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
if ([string]::IsNullOrWhiteSpace($PackageRoot)) {
    $ExtractedPackageRoot = Join-Path $ProjectRoot "obs-glassveil"
    if (Test-Path -LiteralPath $ExtractedPackageRoot) {
        $PackageRoot = $ExtractedPackageRoot
    } else {
        $PackageRoot = Join-Path (Split-Path -Parent $ProjectRoot) "..\outputs\obs-glassveil-windows-x64\obs-glassveil"
    }
}

$PackageRoot = [System.IO.Path]::GetFullPath($PackageRoot)
$TargetRoot = Join-Path $ObsPluginRoot "obs-glassveil"

if (Get-Process -Name "obs64" -ErrorAction SilentlyContinue) {
    throw "OBS is currently running. Close OBS before installing Glassveil."
}

$RequiredFiles = @(
    "bin\64bit\obs-glassveil.dll",
    "data\effects\glassveil.effect",
    "data\locale\en-US.ini"
)

foreach ($RelativePath in $RequiredFiles) {
    $SourcePath = Join-Path $PackageRoot $RelativePath
    if (-not (Test-Path -LiteralPath $SourcePath)) {
        throw "Package file not found: $SourcePath"
    }
}

New-Item -ItemType Directory -Force -Path `
    (Join-Path $TargetRoot "bin\64bit"), `
    (Join-Path $TargetRoot "data\effects"), `
    (Join-Path $TargetRoot "data\locale") | Out-Null

foreach ($RelativePath in $RequiredFiles) {
    Copy-Item -LiteralPath (Join-Path $PackageRoot $RelativePath) -Destination (Join-Path $TargetRoot $RelativePath) -Force
}

Write-Host "Installed Glassveil to $TargetRoot"
foreach ($RelativePath in $RequiredFiles) {
    $Hash = Get-FileHash (Join-Path $TargetRoot $RelativePath) -Algorithm SHA256
    [pscustomobject]@{ File = $RelativePath; SHA256 = $Hash.Hash }
}
