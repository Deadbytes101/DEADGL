$ErrorActionPreference = 'Stop'

function HashScene($Scene) {
    $line = .\build\deadgl.exe hash $Scene
    if ($LASTEXITCODE -ne 0) { throw "hash failed: $Scene" }
    return (($line -split '\s+')[0]).Trim()
}

function CheckDgbParity($Name, $Scene) {
    New-Item -ItemType Directory -Force -Path 'build\dgb_parity' | Out-Null
    $dgb = "build\dgb_parity\$Name.dgb"
    $unpacked = "build\dgb_parity\$Name.unpack.dgl"
    .\build\deadgl.exe pack $Scene -o $dgb
    if ($LASTEXITCODE -ne 0) { throw "pack failed: $Scene" }
    .\build\deadgl.exe disasm $dgb > "build\dgb_parity\$Name.disasm"
    if ($LASTEXITCODE -ne 0) { throw "disasm failed: $dgb" }
    .\build\deadgl.exe unpack $dgb -o $unpacked
    if ($LASTEXITCODE -ne 0) { throw "unpack failed: $dgb" }
    $a = HashScene $Scene
    $b = HashScene $unpacked
    if ($a -ne $b) { throw "dgb parity hash moved: $Name $a $b" }
    Write-Host "DGB PARITY $Name $a"
}

if (!(Test-Path .\build\deadgl.exe)) { throw 'missing build\deadgl.exe; run make first' }

CheckDgbParity 'near_clip' 'examples\near_clip.dgl'
CheckDgbParity 'command_machine' 'examples\command_machine.dgl'

Write-Host 'DGB proof parity locked.'
