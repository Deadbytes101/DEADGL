$ErrorActionPreference = 'Stop'

$repo = 'Deadbytes101/DEADGL'
$version = '1.1.0'
$tag = "v$version"
$dist = "dist\deadgl-$version"
$archive = "dist\deadgl-$version-source.zip"
$description = 'CPU framebuffer command machine. .dgl scripts in, pixels and proof hashes out.'
$topics = 'c99,framebuffer,rasterizer,software-renderer,ppm,deterministic,graphics'

function Run-Checked {
    param(
        [Parameter(Mandatory = $true)]
        [string]$File,
        [string[]]$NativeArgs = @()
    )
    & $File @NativeArgs
    if ($LASTEXITCODE -ne 0) {
        $all = @($File) + $NativeArgs
        throw "command failed: $($all -join ' ')"
    }
}

Run-Checked 'git' @('pull')
Run-Checked 'powershell' @('-ExecutionPolicy', 'Bypass', '-File', 'scripts\release.ps1')

if (!(Get-Command gh -ErrorAction SilentlyContinue)) {
    throw 'GitHub CLI not found. Install gh or use manual release upload.'
}

Run-Checked 'gh' @('auth', 'status')
Run-Checked 'gh' @('repo', 'edit', $repo, '--description', $description, '--add-topic', $topics)

$exists = $false
& gh release view $tag --repo $repo *> $null
if ($LASTEXITCODE -eq 0) {
    $exists = $true
}

if ($exists) {
    Run-Checked 'gh' @('release', 'upload', $tag,
        "$dist\deadgl-windows.exe",
        "$dist\benchmark.txt",
        "$dist\command_machine.ppm",
        "$dist\command_machine.proof",
        "$dist\SHA256SUMS.txt",
        $archive,
        '--repo', $repo,
        '--clobber')
} else {
    Run-Checked 'gh' @('release', 'create', $tag,
        "$dist\deadgl-windows.exe",
        "$dist\benchmark.txt",
        "$dist\command_machine.ppm",
        "$dist\command_machine.proof",
        "$dist\SHA256SUMS.txt",
        $archive,
        '--repo', $repo,
        '--title', "DEADGL $tag",
        '--notes-file', 'docs\RELEASE_V1.1.0.md')
}

Write-Host 'DEADGL publish complete.'
Write-Host "repo    : https://github.com/$repo"
Write-Host "release : https://github.com/$repo/releases/tag/$tag"
