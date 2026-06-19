param(
  [Parameter(Mandatory = $true)][string]$Version,
  [switch]$Publish
)
$ErrorActionPreference = 'Stop'
function Check-Last($what) { if ($LASTEXITCODE -ne 0) { throw "failed: $what" } }
function Repl($path, $pattern, $replacement) { $t = Get-Content $path -Raw; $n = $t -replace $pattern, $replacement; if ($n -ne $t) { Set-Content $path $n -NoNewline } }
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw "bad version: $Version" }
$tag = "v$Version"
if (git status --porcelain) { throw 'working tree is dirty' }
cmd /c "gh release view $tag --repo Deadbytes101/DEADGL >NUL 2>NUL"
if ($LASTEXITCODE -eq 0) { throw "release already exists: $tag" }
cmd /c "git rev-parse --verify refs/tags/$tag >NUL 2>NUL"
if ($LASTEXITCODE -eq 0) { throw "tag already exists: $tag" }
Repl include\deadgl.h '#define DGL_VERSION "[^"]+"' "#define DGL_VERSION `"$Version`""
Repl tests\test_deadgl.c 'strcmp\(dgl_version\(\), "[^"]+"\)' "strcmp(dgl_version(), `"$Version`")"
Repl CMakeLists.txt 'project\(DEADGL VERSION [0-9]+\.[0-9]+\.[0-9]+' "project(DEADGL VERSION $Version"
Repl docs\BUILD.md 'dist/deadgl-[0-9]+\.[0-9]+\.[0-9]+' "dist/deadgl-$Version"
Repl docs\STATE.md 'Current cut: `[^`]+`' "Current cut: ``$Version``"
$notes = "docs\RELEASE_V$Version.md"
if (!(Test-Path $notes)) { @("# DEADGL $tag", '', 'Maintenance release cut.', '', '## Changed', '', '- version bump', '- one-command release path verified') | Set-Content $notes }
& make clean test
Check-Last 'make clean test'
$env:DEADGL_RELEASE_VERSION = $Version
& powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\release.ps1
Check-Last 'release.ps1'
if ((.\build\deadgl.exe --version) -ne "DEADGL $Version") { throw 'version check failed' }
git add include\deadgl.h tests\test_deadgl.c CMakeLists.txt docs\BUILD.md docs\STATE.md $notes
if (git status --porcelain) { & git commit -m "cut $tag"; Check-Last 'git commit' }
& git tag $tag
Check-Last 'git tag'
if ($Publish) {
  & git push origin main
  Check-Last 'git push origin main'
  & git push origin $tag
  Check-Last 'git push origin tag'
  & gh release create $tag ".\dist\deadgl-$Version\*" ".\dist\deadgl-$Version-source.zip" --repo Deadbytes101/DEADGL --title "DEADGL $tag" --notes-file $notes
  Check-Last 'gh release create'
}
Write-Host "DEADGL cut complete: $tag"
