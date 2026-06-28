$ErrorActionPreference = 'Stop'

function Check-Last($what) {
    if ($LASTEXITCODE -ne 0) { throw "failed: $what" }
}

function Read-VersionFromHeader {
    $header = Get-Content .\include\deadgl.h -Raw
    if ($header -notmatch '#define\s+DGL_VERSION\s+"([^"]+)"') { throw 'missing DGL_VERSION in include\deadgl.h' }
    return $Matches[1]
}

function Need-Match($name, $actual, $expected) {
    if ($actual -ne $expected) { throw "$name mismatch: expected $expected got $actual" }
    Write-Host "FINAL $name $actual"
}

function Need-Text($path, $pattern, $name) {
    $text = Get-Content $path -Raw
    if ($text -notmatch $pattern) { throw "missing $name in $path" }
    Write-Host "FINAL $name ok"
}

$version = Read-VersionFromHeader
$tag = "v$version"
$notes = "docs\RELEASE_V$version.md"

if (!(Test-Path $notes)) { throw "missing release notes: $notes" }

Need-Text .\CMakeLists.txt "project\(DEADGL VERSION $([regex]::Escape($version)) LANGUAGES C\)" 'cmake version'
Need-Text .\tests\test_deadgl.c "strcmp\(dgl_version\(\), \"$([regex]::Escape($version))\"\)" 'core test version'

make clean test
Check-Last 'make clean test'

$binaryVersion = .\build\deadgl.exe --version
Need-Match 'binary version' $binaryVersion "DEADGL $version"

powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\golden.ps1
Check-Last 'golden proof'

powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\parser_probe.ps1
Check-Last 'parser probe'

powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\dgb_parity.ps1
Check-Last 'dgb parity'

powershell -NoProfile -ExecutionPolicy Bypass -Command "if ((Get-Content .\examples\raster_truth.hash -Raw) -ne (Get-Content .\build\raster_truth.hash -Raw)) { throw 'raster truth hash mismatch' }"
Check-Last 'raster truth hash'

Write-Host "DEADGL final gate passed for $tag"
