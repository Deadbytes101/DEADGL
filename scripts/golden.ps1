$ErrorActionPreference = 'Stop'

function NeedHash {
    param(
        [string]$Name,
        [string]$Actual,
        [string]$Expected
    )
    if ($Actual -ne $Expected) {
        throw "golden hash moved: $Name expected $Expected got $Actual"
    }
    Write-Host "GOLDEN $Name $Actual"
}

function SceneHash {
    param([string]$Scene)
    $line = .\build\deadgl.exe hash $Scene
    if ($LASTEXITCODE -ne 0) { throw "hash failed: $Scene" }
    return (($line -split '\s+')[0]).Trim()
}

function DemoHash {
    param([string]$Name)
    $out = .\build\deadgl.exe demo $Name -o "build\golden_$Name.ppm"
    if ($LASTEXITCODE -ne 0) { throw "demo failed: $Name" }
    foreach ($line in $out) {
        if ($line -match '^hash\s+([0-9a-f]+)$') { return $Matches[1] }
    }
    throw "demo hash missing: $Name"
}

if (!(Test-Path .\build\deadgl.exe)) { throw 'missing build\deadgl.exe; run make first' }

NeedHash 'cube' (DemoHash 'cube') '95689452fd3b5017'
NeedHash 'command_machine' (SceneHash 'examples\command_machine.dgl') '7dd8eb60b1510af7'
NeedHash 'near_clip' (SceneHash 'examples\near_clip.dgl') '33dd8e01461d1f5c'

Write-Host 'GOLDEN proof hashes locked.'
