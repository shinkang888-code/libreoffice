# lofice Icon Generator
# 마스터 로고를 기반으로 앱·파일 아이콘 PNG/SVG 생성

param(
    [string]$LogoMaster = (Join-Path $PSScriptRoot "..\assets\lofice-logo-master.png"),
    [string]$OutputDir  = (Join-Path $PSScriptRoot "..\assets\icons"),
    [int[]]$Sizes       = @(16, 24, 32, 48, 64, 128, 256, 512)
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path $LogoMaster)) {
    Write-Error "Logo master not found: $LogoMaster"
}

New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null

Add-Type -AssemblyName System.Drawing

function Resize-Logo {
    param([int]$Size, [string]$OutPath)
    $src = [System.Drawing.Image]::FromFile((Resolve-Path $LogoMaster))
    $bmp = New-Object System.Drawing.Bitmap $Size, $Size
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $g.DrawImage($src, 0, 0, $Size, $Size)
    $g.Dispose()
    $bmp.Save($OutPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
    $src.Dispose()
    Write-Host "Generated: $OutPath"
}

foreach ($size in $Sizes) {
    Resize-Logo -Size $size -OutPath (Join-Path $OutputDir "lofice_${size}.png")
}

# SVG wrapper (PNG embed) — 16/32/48 mainapp 교체용
$svg16 = @"
<svg viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <image width="16" height="16" xlink:href="lofice_16.png"/>
</svg>
"@
$svg32 = @"
<svg viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <image width="32" height="32" xlink:href="lofice_32.png"/>
</svg>
"@
Set-Content -Path (Join-Path $OutputDir "mainapp_16.svg") -Value $svg16 -Encoding UTF8
Set-Content -Path (Join-Path $OutputDir "mainapp_32.svg") -Value $svg32 -Encoding UTF8

Write-Host ""
Write-Host "Icon generation complete: $OutputDir"
