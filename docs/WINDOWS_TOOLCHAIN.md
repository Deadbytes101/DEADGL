# Windows toolchain

w64devkit can build DEADGL, but some builds do not ship ASan/UBSan runtime libraries.

That is fine.

`make sanitize` now probes the compiler first. If the runtime is missing, it prints:

```text
SKIP sanitize: sanitizer runtime unavailable
```

Then it cleans the build directory so normal release builds are not poisoned by sanitizer object files.

Recommended Windows path is an sh-capable toolchain:

```sh
git pull
make clean test
sh scripts/release.sh
```

Expected output:

```text
DEADGL local release cut complete.
dist folder : dist/deadgl-11.0.0
source tar  : dist/deadgl-11.0.0-source.tar.gz
```
