# Windows toolchain

w64devkit can build DEADGL, but some builds do not ship ASan/UBSan runtime libraries.

That is fine.

`make sanitize` now probes the compiler first. If the runtime is missing, it prints:

```text
SKIP sanitize: sanitizer runtime unavailable
```

Then it cleans the build directory so normal release builds are not poisoned by sanitizer object files.

Recommended Windows release path:

```powershell
git pull
powershell -ExecutionPolicy Bypass -File scripts\release.ps1
```

Expected output:

```text
DEADGL local release cut complete.
dist folder : dist\deadgl-1.1.0
source zip  : dist\deadgl-1.1.0-source.zip
```
