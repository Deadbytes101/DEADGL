$ErrorActionPreference = 'Stop'

function New-BadScene($Name, $Lines) {
    New-Item -ItemType Directory -Force -Path 'build\parser_probe' | Out-Null
    $path = "build\parser_probe\$Name.dgl"
    $Lines | Set-Content $path
    return $path
}

function Check-BadScene($Name, $Lines, $Text) {
    $scene = New-BadScene $Name $Lines
    $out = "build\parser_probe\$Name.ppm"
    $stdout = "build\parser_probe\$Name.stdout"
    $stderr = "build\parser_probe\$Name.stderr"
    & .\build\deadgl.exe run $scene -o $out 1> $stdout 2> $stderr
    if ($LASTEXITCODE -eq 0) { throw "expected parser failure: $Name" }
    $err = Get-Content $stderr -Raw
    if ($err -notmatch [regex]::Escape($Text)) { throw "missing parser text: $Text" }
    Write-Host "PARSER $Name $Text"
}

if (!(Test-Path .\build\deadgl.exe)) { throw 'missing build\deadgl.exe; run make first' }

Check-BadScene 'bad_command' @('canvas 16 16', 'bogus') 'unknown command'
Check-BadScene 'bad_arity' @('canvas 16 16', 'clear') 'clear wants: clear COLOR'
Check-BadScene 'bad_integer' @('canvas 16 16', 'line x 0 4 4 #ffffff') 'bad integer'
Check-BadScene 'bad_float' @('canvas 16 16', 'camera 0 0 x 0 0 0 70') 'bad camera float'
Check-BadScene 'bad_color' @('canvas 16 16', 'clear badcolor') 'bad color'
Check-BadScene 'bad_vertex_index' @('canvas 16 16', 'meshtri 0 1 2 #ffffff') 'bad meshtri index'

Write-Host 'PARSER diagnostics locked.'
