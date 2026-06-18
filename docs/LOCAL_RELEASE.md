# Local release

GitHub Actions can be unavailable at the account level. DEADGL does not depend on Actions for a release cut.

Linux or MSYS2:

```sh
chmod +x scripts/debug_probe.sh scripts/bench.sh scripts/release.sh
scripts/debug_probe.sh
scripts/bench.sh
scripts/release.sh
```

Windows PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\release.ps1
```

The release scripts create:

```text
dist/deadgl-1.1.0/
dist/deadgl-1.1.0-source.tar.gz
dist/deadgl-1.1.0-source.zip
```

The dist folder contains the binary, benchmark report, PPM render, proof file, release notes, and SHA256 sums.

Publish without Actions:

```sh
gh release create v1.1.0 dist/deadgl-1.1.0/* dist/deadgl-1.1.0-source.tar.gz --title "DEADGL v1.1.0" --notes-file docs/RELEASE_V1.1.0.md
```

Or open GitHub Releases in the browser, click `Draft a new release`, use tag `v1.1.0`, and upload files from `dist/deadgl-1.1.0`.
