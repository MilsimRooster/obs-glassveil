param(
    [string]$Configuration = "RelWithDebInfo",
    [string]$OutputRoot = ""
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path (Split-Path -Parent $ProjectRoot) "..\outputs\obs-glassveil-windows-x64"
}

$OutputRoot = [System.IO.Path]::GetFullPath($OutputRoot)
$PluginRoot = Join-Path $OutputRoot "obs-glassveil"
$ZipPath = "$OutputRoot.zip"

$DllPath = Join-Path $ProjectRoot "build_x64\$Configuration\obs-glassveil.dll"
$EffectPath = Join-Path $ProjectRoot "data\effects\glassveil.effect"
$LocalePath = Join-Path $ProjectRoot "data\locale\en-US.ini"
$ReadmePath = Join-Path $ProjectRoot "README.md"
$ReleaseNotesPath = Join-Path $ProjectRoot "RELEASE_NOTES.md"
$ContributingPath = Join-Path $ProjectRoot "CONTRIBUTING.md"
$NoticePath = Join-Path $ProjectRoot "NOTICE.md"
$QuickstartPath = Join-Path $ProjectRoot "docs\OBS-QUICKSTART.md"
$InstallScriptPath = Join-Path $ProjectRoot "tools\install-windows.ps1"
$UninstallScriptPath = Join-Path $ProjectRoot "tools\uninstall-windows.ps1"

foreach ($RequiredPath in @($DllPath, $EffectPath, $LocalePath, $ReadmePath, $ReleaseNotesPath, $ContributingPath, $NoticePath, $QuickstartPath, $InstallScriptPath, $UninstallScriptPath)) {
    if (-not (Test-Path -LiteralPath $RequiredPath)) {
        throw "Required file not found: $RequiredPath"
    }
}

New-Item -ItemType Directory -Force -Path `
    (Join-Path $PluginRoot "bin\64bit"), `
    (Join-Path $PluginRoot "data\effects"), `
    (Join-Path $PluginRoot "data\locale") | Out-Null

Copy-Item -LiteralPath $DllPath -Destination (Join-Path $PluginRoot "bin\64bit\obs-glassveil.dll") -Force
Copy-Item -LiteralPath $EffectPath -Destination (Join-Path $PluginRoot "data\effects\glassveil.effect") -Force
Copy-Item -LiteralPath $LocalePath -Destination (Join-Path $PluginRoot "data\locale\en-US.ini") -Force
Copy-Item -LiteralPath $ReadmePath -Destination (Join-Path $OutputRoot "README.md") -Force
Copy-Item -LiteralPath $ReleaseNotesPath -Destination (Join-Path $OutputRoot "RELEASE_NOTES.md") -Force
Copy-Item -LiteralPath $ContributingPath -Destination (Join-Path $OutputRoot "CONTRIBUTING.md") -Force
Copy-Item -LiteralPath $NoticePath -Destination (Join-Path $OutputRoot "NOTICE.md") -Force

New-Item -ItemType Directory -Force -Path (Join-Path $OutputRoot "tools"), (Join-Path $OutputRoot "docs") | Out-Null
Copy-Item -LiteralPath $InstallScriptPath -Destination (Join-Path $OutputRoot "tools\install-windows.ps1") -Force
Copy-Item -LiteralPath $UninstallScriptPath -Destination (Join-Path $OutputRoot "tools\uninstall-windows.ps1") -Force
Copy-Item -LiteralPath $QuickstartPath -Destination (Join-Path $OutputRoot "docs\OBS-QUICKSTART.md") -Force

Compress-Archive -Path `
    $PluginRoot, `
    (Join-Path $OutputRoot "tools"), `
    (Join-Path $OutputRoot "docs"), `
    (Join-Path $OutputRoot "README.md"), `
    (Join-Path $OutputRoot "CONTRIBUTING.md"), `
    (Join-Path $OutputRoot "NOTICE.md"), `
    (Join-Path $OutputRoot "RELEASE_NOTES.md") `
    -DestinationPath $ZipPath -Force

Write-Host "Packaged Glassveil:"
Write-Host "  Folder: $OutputRoot"
Write-Host "  Zip:    $ZipPath"
Get-ChildItem -Recurse -File $OutputRoot | Select-Object FullName, Length
