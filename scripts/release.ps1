$ErrorActionPreference = 'Stop'
$version = '2.0.0'
$tag = "v$version"
$dist = "dist\deadgl-$version"
$archive = "dist\deadgl-$version-source.zip"
$scene = 'examples\command_machine.dgl'
$clipScene = 'examples\near_clip.dgl'
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
.\build\deadgl.exe prove $clipScene -o "$dist\near_clip.ppm" -p "$dist\near_clip.proof"
.\build\deadgl.exe inspect $clipScene > "$dist\near_clip.main.inspect"
.\build\deadgl-inspect.exe $clipScene > "$dist\near_clip.inspect"
.\build\deadgl.exe hash $scene > "$dist\command_machine.hash"
.\build\deadgl.exe hash $clipScene > "$dist\near_clip.hash"
Copy-Item .\build\deadgl.exe "$dist\deadgl-windows.exe"
Copy-Item .\build\deadgl-inspect.exe "$dist\deadgl-inspect-windows.exe"
Copy-Item README.md, MANIFESTO.md, LICENSE, PROOF.md $dist
Copy-Item docs\RELEASE_V2.0.0.md "$dist\RELEASE_NOTES.md"
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
Write-Host "gh release create $tag $dist/* $archive --title 'DEADGL $tag' --notes-file docs/RELEASE_V2.0.0.md"
