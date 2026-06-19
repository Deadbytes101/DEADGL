param(
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [switch]$Publish
)

$ErrorActionPreference = 'Stop'

function Replace-InFile {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Pattern,
        [Parameter(Mandatory = $true)][string]$Replacement
    )
    $text = Get-Content $Path -Raw
    $next = $text -replace $Pattern, $Replacement
    if ($next -ne $text) {
        Set-Content -Path $Path -Value $next -NoNewline
    }
}

function Run-Command {
    param(
        [Parameter(Mandatory = $true)][string]$File,
        [string[]]$Args = @()
    )
    & $File @Args
    if ($LASTEXITCODE -ne 0) {
        throw "command failed: $File $($Args -join ' ')"
    }
}

if ($Version -notmatch '^\d+\.\d+\.\d+$') {
    throw "bad version: $Version"
}

$tag = "v$Version"
$dirty = git status --porcelain
if ($dirty) {
    throw "working tree is dirty; commit/stash changes first"
}

gh release view $tag --repo Deadbytes101/DEADGL *> $null
if ($LASTEXITCODE -eq 0) {
    throw "release already exists: $tag"
}

git rev-parse $tag *> $null
if ($LASTEXITCODE -eq 0) {
    throw "tag already exists: $tag"
}

Replace-InFile include\deadgl.h '#define DGL_VERSION "[^"]+"' "#define DGL_VERSION `"$Version`""
Replace-InFile tests\test_deadgl.c 'strcmp\(dgl_version\(\), "[^"]+"\)' "strcmp(dgl_version(), `"$Version`")"
Replace-InFile CMakeLists.txt 'project\(DEADGL VERSION [0-9]+\.[0-9]+\.[0-9]+' "project(DEADGL VERSION $Version"
Replace-InFile docs\BUILD.md 'dist/deadgl-[0-9]+\.[0-9]+\.[0-9]+' "dist/deadgl-$Version"
Replace-InFile docs\STATE.md 'Current cut: `[^`]+`' "Current cut: ``$Version``"

$notes = "docs\RELEASE_V$Version.md"
if (!(Test-Path $notes)) {
    @(
        "# DEADGL $tag",
        '',
        'Maintenance release cut.',
        '',
        '## Changed',
        '',
        '- version bump',
        '- release automation path verified',
        '',
        '## Run',
        '',
        '```sh',
        './deadgl --version',
        './deadgl inspect examples/near_clip.dgl',
        './deadgl run examples/command_machine.dgl -o command_machine.ppm',
        '```'
    ) | Set-Content $notes
}

Run-Command make @('clean', 'test')
$env:DEADGL_RELEASE_VERSION = $Version
Run-Command powershell @('-ExecutionPolicy', 'Bypass', '-File', '.\scripts\release.ps1')

$check = .\build\deadgl.exe --version
if ($check -ne "DEADGL $Version") {
    throw "version check failed: $check"
}

git add include\deadgl.h tests\test_deadgl.c CMakeLists.txt docs\BUILD.md docs\STATE.md $notes
$changed = git status --porcelain
if ($changed) {
    Run-Command git @('commit', '-m', "cut $tag")
}
Run-Command git @('tag', $tag)

if ($Publish) {
    Run-Command git @('push', 'origin', 'main')
    Run-Command git @('push', 'origin', $tag)
    Run-Command gh @('release', 'create', $tag, ".\dist\deadgl-$Version\*", ".\dist\deadgl-$Version-source.zip", '--repo', 'Deadbytes101/DEADGL', '--title', "DEADGL $tag", '--notes-file', $notes)
}

Write-Host "DEADGL cut complete: $tag"
if (!$Publish) {
    Write-Host "Publish with:"
    Write-Host "git push origin main"
    Write-Host "git push origin $tag"
    Write-Host "gh release create $tag .\dist\deadgl-$Version\* .\dist\deadgl-$Version-source.zip --repo Deadbytes101/DEADGL --title 'DEADGL $tag' --notes-file $notes"
}
