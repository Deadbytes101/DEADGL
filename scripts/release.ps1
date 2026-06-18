$ErrorActionPreference = 'Stop'

$version = '1.1.0'
$tag = "v$version"
$distRoot = 'dist'
$dist = Join-Path $distRoot "deadgl-$version"
$archive = Join-Path $distRoot "deadgl-$version-source.zip"

if (Test-Path $distRoot) { Remove-Item $distRoot -Recurse -Force }
New-Item -ItemType Directory -Force -Path $dist | Out-Null

make clean test
make clean sanitize
make

.\build\deadgl.exe prove examples\command_machine.dgl -o "$dist\command_machine.ppm" -p "$dist\command_machine.proof"
$env:DEADGL_BENCH_LOOPS = '200'
sh scripts/bench.sh examples/command_machine.dgl

Copy-Item .\build\deadgl.exe "$dist\deadgl-windows.exe"
Copy-Item .\build\benchmark.txt "$dist\benchmark.txt"
Copy-Item README.md, MANIFESTO.md, LICENSE, PROOF.md $dist
Copy-Item docs\RELEASE_V1.1.0.md "$dist\RELEASE_NOTES.md"

Compress-Archive -Path * -DestinationPath $archive -Force

$shaFile = Join-Path $dist 'SHA256SUMS.txt'
Get-ChildItem $dist -File | ForEach-Object {
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
