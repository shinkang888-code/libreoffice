# lofice Rebranding Script
# LibreOffice 사용자 표시 문자열 → lofice 일괄 치환
# 주의: org.libreoffice.* UNO 서비스 ID는 호환성 위해 제외

param(
    [string]$Root = (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent),
    [switch]$DryRun,
    [ValidateSet('Display', 'Headers', 'All')]
    [string]$Tier = 'Display'
)

$ErrorActionPreference = 'Stop'

# ── 치환 규칙 (순서 중요: 긴 패턴 먼저) — 배열 사용 (PowerShell 해시 대소문자 충돌 방지) ──
$DisplayReplacements = @(
    @{ From = 'LibreOfficeDev';           To = 'loficeDev' }
    @{ From = 'Libre Office';             To = 'lofice' }
    @{ From = 'LibreOffice';              To = 'lofice' }
    @{ From = 'libreoffice';              To = 'lofice' }
    @{ From = 'LIBREOFFICE';              To = 'LOFICE' }
    @{ From = 'The Document Foundation';  To = 'Lonex. Inc' }
    @{ From = 'documentfoundation.org';   To = 'lofice.io' }
    @{ From = 'Document Foundation';      To = 'Lonex. Inc' }
)

$HeaderReplacements = @(
    @{ From = 'This file is part of the LibreOffice project.'; To = 'This file is part of the lofice project.' }
    @{ From = 'LibreOffice project'; To = 'lofice project' }
)

# ── 제외 경로 (UNO 호환·빌드 아티팩트) ──
$ExcludeDirPatterns = @(
    '\.git',
    'workdir',
    'instdir',
    'lofice\assets',
    'lofice\scripts\generated-icons'
)

$ExcludeFilePatterns = @(
    'rebrand-lofice\.ps1$',
    'brand-manifest\.json$',
    'LICENSE\.md$',
    'COPYRIGHT-KR\.md$',
    'phase1-architecture-mapping\.md$'
)

# org.libreoffice.* 는 Tier=All 에서도 UNO 서비스 ID 보호
$UnoServicePattern = 'org\.libreoffice\.|vnd\.libreoffice\.|com\.sun\.star\.comp\.(?!lofice)'

# Android LOK JNI — Java 클래스·네이티브 심볼명 유지 (strings.xml 등 res/ 만 Display 적용)
$ExcludeDirPatterns += 'android\\Bootstrap'
$ExcludeDirPatterns += 'android\\source\\src'

function Test-ExcludedPath {
    param([string]$Path)
    $normalized = $Path -replace '\\', '/'
    foreach ($p in $ExcludeDirPatterns) {
        $pat = ($p -replace '\\', '/')
        if ($normalized -like "*$pat*") { return $true }
    }
    foreach ($p in $ExcludeFilePatterns) {
        if ($Path -match $p) { return $true }
    }
    return $false
}

function Get-ReplacementMap {
    param([string]$TierName)
    $list = @()
    if ($TierName -in @('Display', 'All')) { $list += $DisplayReplacements }
    if ($TierName -in @('Headers', 'All')) { $list += $HeaderReplacements }
    return $list
}

function Protect-UnoLines {
    param([string[]]$Lines)
    $result = @()
    foreach ($line in $Lines) {
        if ($line -match $UnoServicePattern) {
            $result += $line  # UNO ID 라인은 치환하지 않음
        } else {
            $result += $line
        }
    }
    return $result -join "`n"
}

$replacements = Get-ReplacementMap -TierName $Tier
$extensions = @('.ulf', '.desktop', '.xml', '.xcu', '.xcs', '.html', '.htm', '.md', '.txt', '.conf', '.ini', '.rc', '.properties', '.json', '.yaml', '.yml', '.ac', '.am', '.mk', '.scp', '.py', '.sh', '.bat', '.ps1', '.java', '.cxx', '.cpp', '.c', '.hxx', '.h', '.idl')

$stats = @{ Files = 0; Changes = 0; Skipped = 0 }

Get-ChildItem -Path $Root -Recurse -File -ErrorAction SilentlyContinue | ForEach-Object {
    $file = $_
    if (Test-ExcludedPath $file.FullName) { $stats.Skipped++; return }
    if ($extensions -notcontains $file.Extension.ToLower()) { return }

    try {
        $content = [System.IO.File]::ReadAllText($file.FullName)
    } catch { return }

    $original = $content
    foreach ($rule in $replacements) {
        $from = $rule.From
        $to = $rule.To
        # UNO 서비스 ID가 포함된 라인은 모든 Tier에서 보호
        $lines = $content -split "`n"
        $newLines = foreach ($line in $lines) {
            if ($line -match $UnoServicePattern) { $line }
            else { $line -replace [regex]::Escape($from), $to }
        }
        $content = $newLines -join "`n"
    }

    if ($content -ne $original) {
        $stats.Files++
        $stats.Changes++
        if (-not $DryRun) {
            [System.IO.File]::WriteAllText($file.FullName, $content, [System.Text.UTF8Encoding]::new($false))
        }
        Write-Host "$(if ($DryRun) {'[DRY] '})Updated: $($file.FullName.Replace($Root, '.'))"
    }
}

Write-Host ""
Write-Host "=== lofice Rebrand Summary (Tier: $Tier) ==="
Write-Host "Files modified: $($stats.Files)"
Write-Host "Skipped paths:  $($stats.Skipped)"
if ($DryRun) { Write-Host "(Dry run — no files written)" }
