# Local release

GitHub Actions can be unavailable at the account level. DEADGL does not depend on Actions for a local release cut.

Linux, MSYS2, Git Bash, or WSL:

```sh
sh scripts/debug_probe.sh
sh scripts/bench.sh
sh scripts/release.sh
```

The release scripts create:

```text
dist/deadgl-11.0.0/
dist/deadgl-11.0.0-source.tar.gz
```

The dist folder contains the binary, benchmark report, PPM render, proof file, release notes, and SHA256 sums.

Publish manually from the generated `dist` folder when the local release cut is clean.
