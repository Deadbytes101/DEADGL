$ErrorActionPreference = 'Stop'
$version = '1.6.0'
$tag = "v$version"
$dist = "dist\deadgl-$version"
$archive = "dist\deadgl-$version-source.zip"
$scene = 'examples\command_machine.dgl'
New-Item -ItemType Directory -Force -Path $dist | Out-Null
Remove-Item $archive -Force -ErrorAction SilentlyContinue
Remove-Item "$dist\*" -Force -ErrorAction SilentlyContinue
make clean test
if ($LASTEXITCODE -ne 0) { throw 'test failed' }
make clean sanitize
make clean
make
if ($LASTEXITCODE -ne 0) { throw 'build failed' }
.\build\deadgl.exe prove $scene -o "$dist\command_machine.ppm" -p "$dist\command_machine.proof"
.\build\deadgl.exe run $scene -o build\bench_warmup.ppm
$loops = 200
$sw = [System.Diagnostics.Stopwatch]::StartNew()
for ($i = 0; $i -lt $loops; $i++) { .\build\deadgl.exe run $scene -o build\bench.ppm }
$sw.Stop()
$totalNs = [int64]($sw.Elapsed.TotalMilliseconds * 1000000.0)
$avgNs = [int64]($totalNs / $loops)
$hashLine = .\build\deadgl.exe hash $scene
$versionLine = .\build\deadgl.exe --version
@('DEADGL BENCHMARK', "version $versionLine", "scene $scene", "loops $loops", "total_ns $totalNs", "avg_ns $avgNs", "hash $hashLine") | Set-Content build\benchmark.txt
Copy-Item .\build\deadgl.exe "$dist\deadgl-windows.exe"
Copy-Item .\build\benchmark.txt "$dist\benchmark.txt"
Copy-Item README.md, MANIFESTO.md, LICENSE, PROOF.md $dist
Copy-Item docs\RELEASE_V1.6.0.md "$dist\RELEASE_NOTES.md"
$sourceItems = @('include','src','tests','examples','docs','scripts','README.md','MANIFESTO.md','Makefile','CMakeLists.txt','LICENSE','PROOF.md') | Where-Object { Test-Path $_ }
Compress-Archive -Path $sourceItems -DestinationPath $archive -Force
$shaFile = "$dist\SHA256SUMS.txt"
Remove-Item $shaFile -Force -ErrorAction SilentlyContinue
Get-ChildItem $dist -File | Where-Object { $_.Name -ne 'SHA256SUMS.txt' } | ForEach-Object { $hash = Get-FileHash $_.FullName -Algorithm SHA256; "$($hash.Hash.ToLower())  $($_.Name)" | Add-Content $shaFile }
$archiveHash = Get-FileHash $archive -Algorithm SHA256
"$($archiveHash.Hash.ToLower())  $(Split-Path $archive -Leaf)" | Add-Content $shaFile
Write-Host "DEADGL local release cut complete."
Write-Host "dist folder : $dist"
Write-Host "source zip  : $archive"
Write-Host "gh release create $tag $dist/* $archive --title 'DEADGL $tag' --notes-file docs/RELEASE_V1.6.0.md"
