$ErrorActionPreference = 'Stop'

function Run-Native {
    param(
        [Parameter(Mandatory = $true, Position = 0)]
        [string]$File,

        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$NativeArgs
    )

    if ($null -eq $NativeArgs) {
        $NativeArgs = @()
    }

    & $File @NativeArgs
    if ($LASTEXITCODE -ne 0) {
        $all = @($File) + $NativeArgs
        throw "command failed: $($all -join ' ')"
    }
}

$version = '1.1.0'
$tag = "v$version"
$distRoot = 'dist'
$dist = Join-Path $distRoot "deadgl-$version"
$archive = Join-Path $distRoot "deadgl-$version-source.zip"
$scene = 'examples\command_machine.dgl'
$loops = 200

if (Test-Path $distRoot) { Remove-Item $distRoot -Recurse -Force }
New-Item -ItemType Directory -Force -Path $dist | Out-Null

Run-Native make clean test
Run-Native make clean sanitize
Run-Native make clean
Run-Native make

if (!(Test-Path .\build\deadgl.exe)) {
    throw 'missing build\deadgl.exe after normal build'
}

Run-Native .\build\deadgl.exe prove $scene -o "$dist\command_machine.ppm" -p "$dist\command_machine.proof"
Run-Native .\build\deadgl.exe run $scene -o build\bench_warmup.ppm

$sw = [System.Diagnostics.Stopwatch]::StartNew()
for ($i = 0; $i -lt $loops; $i++) {
    Run-Native .\build\deadgl.exe run $scene -o build\bench.ppm
}
$sw.Stop()
$totalNs = [int64]($sw.Elapsed.TotalMilliseconds * 1000000.0)
$avgNs = [int64]($totalNs / $loops)
$avgMs = [double]$avgNs / 1000000.0
$hashLine = .\build\deadgl.exe hash $scene
$versionLine = .\build\deadgl.exe --version
$bench = @(
    'DEADGL BENCHMARK',
    "version $versionLine",
    "scene $scene",
    "loops $loops",
    "total_ns $totalNs",
    "avg_ns $avgNs",
    ('avg_ms {0:N3}' -f $avgMs),
    "hash $hashLine",
    "shell PowerShell",
    "system $([System.Environment]::OSVersion.VersionString)"
)
$bench | Set-Content build\benchmark.txt

Copy-Item .\build\deadgl.exe "$dist\deadgl-windows.exe"
Copy-Item .\build\benchmark.txt "$dist\benchmark.txt"
Copy-Item README.md, MANIFESTO.md, LICENSE, PROOF.md $dist
Copy-Item docs\RELEASE_V1.1.0.md "$dist\RELEASE_NOTES.md"

$sourceItems = @('include','src','tests','examples','docs','scripts','.github','README.md','MANIFESTO.md','Makefile','CMakeLists.txt','LICENSE','PROOF.md')
Compress-Archive -Path $sourceItems -DestinationPath $archive -Force

$shaFile = Join-Path $dist 'SHA256SUMS.txt'
if (Test-Path $shaFile) { Remove-Item $shaFile -Force }
Get-ChildItem $dist -File | Where-Object { $_.Name -ne 'SHA256SUMS.txt' } | ForEach-Object {
    $hash = Get-FileHash $_.FullName -Algorithm SHA256
    "$($hash.Hash.ToLower())  $($_.Name)" | Add-Content $shaFile
}
$archiveHash = Get-FileHash $archive -Algorithm SHA256
"$($archiveHash.Hash.ToLower())  $(Split-Path $archive -Leaf)" | Add-Content $shaFile

Write-Host "DEADGL local release cut complete."
Write-Host "dist folder : $dist"
Write-Host "source zip  : $archive"
Write-Host ""
Write-Host "Optional publish with GitHub CLI:"
Write-Host "gh release create $tag $dist/* $archive --title 'DEADGL $tag' --notes-file docs/RELEASE_V1.1.0.md"
