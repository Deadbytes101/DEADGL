param(
  [Parameter(Mandatory = $true)][string]$Version,
  [switch]$Publish
)
$ErrorActionPreference = 'Stop'
function Check($what) { if ($LASTEXITCODE -ne 0) { throw "failed: $what" } }
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw "bad version: $Version" }
$tag = "v$Version"
$notes = "docs\RELEASE_V$Version.md"
if (!(Test-Path $notes)) { throw "missing notes: $notes" }
cmd /c "gh release view $tag --repo Deadbytes101/DEADGL >NUL 2>NUL"
if ($LASTEXITCODE -eq 0) { throw "release already exists: $tag" }
cmd /c "git rev-parse --verify refs/tags/$tag >NUL 2>NUL"
if ($LASTEXITCODE -eq 0) { throw "tag already exists: $tag" }
$env:DEADGL_RELEASE_VERSION = $Version
& make clean test
Check 'make clean test'
& powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\release.ps1
Check 'release.ps1'
if ((.\build\deadgl.exe --version) -ne "DEADGL $Version") { throw 'version check failed' }
& git add include\deadgl.h tests\test_deadgl.c CMakeLists.txt docs\BUILD.md docs\STATE.md $notes
Check 'git add'
if (git status --porcelain) { & git commit -m "cut $tag"; Check 'git commit' }
& git tag $tag
Check 'git tag'
if ($Publish) {
  & git push origin main
  Check 'git push origin main'
  & git push origin $tag
  Check 'git push origin tag'
  & gh release create $tag ".\dist\deadgl-$Version\*" ".\dist\deadgl-$Version-source.zip" --repo Deadbytes101/DEADGL --title "DEADGL $tag" --notes-file $notes
  Check 'gh release create'
}
Write-Host "DEADGL resumed cut complete: $tag"
