$ErrorActionPreference = 'Stop'
function Check-Last($what) { if ($LASTEXITCODE -ne 0) { throw "failed: $what" } }

$version = $env:DEADGL_RELEASE_VERSION
if (-not $version -and $env:GITHUB_REF_NAME) { $version = $env:GITHUB_REF_NAME -replace '^v', '' }
if (-not $version) {
    $header = Get-Content .\include\deadgl.h -Raw
    if ($header -match '#define\s+DGL_VERSION\s+"([^"]+)"') { $version = $Matches[1] }
}
if (-not $version) { throw 'cannot resolve release version' }

$tag = "v$version"
$dist = "dist\deadgl-$version"
$archive = "dist\deadgl-$version-source.zip"
$scene = 'examples\command_machine.dgl'
$clipScene = 'examples\near_clip.dgl'
$notes = "docs\RELEASE_V$version.md"
if (-not (Test-Path $notes)) {
    New-Item -ItemType Directory -Force -Path build | Out-Null
    $notes = 'build\RELEASE_NOTES.generated.md'
    @("# DEADGL $tag", '', 'Automated release cut.', '', "Built from tag $tag.") | Set-Content $notes
}

New-Item -ItemType Directory -Force -Path $dist | Out-Null
Remove-Item $archive -Force -ErrorAction SilentlyContinue
Remove-Item "$dist\*" -Force -ErrorAction SilentlyContinue
make clean test
Check-Last 'make clean test'
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\golden.ps1
Check-Last 'golden proof'
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\parser_probe.ps1
Check-Last 'parser probe'
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\dgb_parity.ps1
Check-Last 'dgb parity'
make clean sanitize
Check-Last 'sanitize'
make clean
Check-Last 'make clean'
make
Check-Last 'make'
$versionLine = .\build\deadgl.exe --version
if ($versionLine -ne "DEADGL $version") { throw "version mismatch: binary says '$versionLine', release is '$tag'" }
.\build\deadgl.exe prove $scene -o "$dist\command_machine.ppm" -p "$dist\command_machine.proof"
Check-Last 'prove command_machine'
.\build\deadgl.exe prove $clipScene -o "$dist\near_clip.ppm" -p "$dist\near_clip.proof"
Check-Last 'prove near_clip'
.\build\deadgl.exe textdemo -o "$dist\textdemo.ppm"
Check-Last 'textdemo package'
.\build\deadgl.exe tiledemo -o "$dist\tiledemo.ppm"
Check-Last 'tiledemo package'
.\build\deadview.exe "$dist\tiledemo.ppm" > "$dist\tiledemo.view"
Check-Last 'deadview package'
.\build\deadpad.exe new "$dist\deadpad_seed.dgl"
Check-Last 'deadpad seed'
.\build\deadpad.exe append "$dist\deadpad_seed.dgl" line 8 8 120 8 0xff8822
Check-Last 'deadpad append'
.\build\deadpad.exe cat "$dist\deadpad_seed.dgl" > "$dist\deadpad_seed.cat"
Check-Last 'deadpad cat'
.\build\deadgl.exe run "$dist\deadpad_seed.dgl" -o "$dist\deadpad_seed.ppm"
Check-Last 'deadpad render'
.\build\deadgl.exe shell "$dist\deadpad_seed.dgl" -o "$dist\shell_file.ppm"
Check-Last 'shell file package'
$shellScript = "$dist\shell.stdin.dgl"
@('canvas 64 64','clear #050608','line 4 4 60 60 #00ff99') | Set-Content $shellScript
.\build\deadgl.exe shell $shellScript -o "$dist\shell.ppm"
Check-Last 'shell package'
.\build\deadgl.exe scenepack $scene -o "$dist\command_machine.dgp"
Check-Last 'scene pack package'
.\build\deadgl.exe sceneunpack "$dist\command_machine.dgp" -o "$dist\command_machine.unpack.dgl"
Check-Last 'scene unpack package'
.\build\deadgl.exe run "$dist\command_machine.unpack.dgl" -o "$dist\command_machine.unpack.ppm"
Check-Last 'scene unpack render'
.\build\deadgl.exe suite $scene -o "$dist\command_machine.suite"
Check-Last 'suite report package'
.\build\deadgl.exe inspect $clipScene > "$dist\near_clip.main.inspect"
Check-Last 'inspect package'
.\build\deadgl-inspect.exe $clipScene > "$dist\near_clip.inspect"
Check-Last 'deadgl-inspect package'
.\build\deadgl.exe audit $scene > "$dist\command_machine.audit"
Check-Last 'audit package'
.\build\deadgl.exe pack $clipScene -o "$dist\near_clip.dgb"
Check-Last 'dgb pack package'
.\build\deadgl.exe disasm "$dist\near_clip.dgb" > "$dist\near_clip.disasm"
Check-Last 'dgb disasm package'
.\build\deadgl.exe unpack "$dist\near_clip.dgb" -o "$dist\near_clip.unpack.dgl"
Check-Last 'dgb unpack package'
.\build\deadgl.exe hash $scene > "$dist\command_machine.hash"
Check-Last 'hash command_machine'
.\build\deadgl.exe hash $clipScene > "$dist\near_clip.hash"
Check-Last 'hash near_clip'
Copy-Item .\build\deadgl.exe "$dist\deadgl-windows.exe"
Copy-Item .\build\deadgl-inspect.exe "$dist\deadgl-inspect-windows.exe"
Copy-Item .\build\deadpad.exe "$dist\deadpad-windows.exe"
Copy-Item .\build\deadview.exe "$dist\deadview-windows.exe"
Copy-Item README.md, MANIFESTO.md, LICENSE, PROOF.md $dist
Copy-Item $notes "$dist\RELEASE_NOTES.md"
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
Write-Host "gh release create $tag $dist/* $archive --title 'DEADGL $tag' --notes-file $notes"
