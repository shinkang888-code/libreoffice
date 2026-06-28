# lofice Icon Theme Deploy
# 생성된 lofice 아이콘을 icon-themes colibre 테마 mainapp 슬롯에 배치

param(
    [string]$RepoRoot = (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent),
    [string]$IconSource = (Join-Path $PSScriptRoot "..\assets\icons"),
    [string[]]$Themes = @('colibre_svg', 'colibre_dark_svg', 'sukapura_svg', 'sukapura_dark_svg', 'breeze_svg', 'breeze_dark_svg')
)

$ErrorActionPreference = 'Stop'

$mainappTargets = @(
    'mainapp_16.svg', 'mainapp_16_8.svg', 'mainapp_32.svg', 'mainapp_32_8.svg'
)

foreach ($theme in $Themes) {
    $themeDir = Join-Path $RepoRoot "icon-themes\$theme\res"
    if (-not (Test-Path $themeDir)) {
        Write-Warning "Theme not found: $themeDir"
        continue
    }
    foreach ($target in $mainappTargets) {
        $dest = Join-Path $themeDir $target
        if (-not (Test-Path $dest)) { continue }
        # 백업
        $bak = "$dest.lofice.bak"
        if (-not (Test-Path $bak)) { Copy-Item $dest $bak -Force }
        # 16/32 크기에 맞는 SVG 복사
        if ($target -match '_32') {
            $src = Join-Path $IconSource 'mainapp_32.svg'
        } else {
            $src = Join-Path $IconSource 'mainapp_16.svg'
        }
        if (Test-Path $src) {
            Copy-Item $src $dest -Force
            Write-Host "Deployed: $theme/$target"
        }
    }
    # SVG embed PNG (mainapp_*.svg → lofice_16.png / lofice_32.png)
    foreach ($png in @('lofice_16.png', 'lofice_32.png')) {
        $pngSrc = Join-Path $IconSource $png
        if (Test-Path $pngSrc) {
            Copy-Item $pngSrc (Join-Path $themeDir $png) -Force
            Write-Host "Deployed: $theme/$png"
        }
    }
}

Write-Host "Icon theme deployment complete."
