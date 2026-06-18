# Build

No package manager. No generated project tree.

```sh
make clean test
make
```

Harder path:

```sh
make clean sanitize
```

CMake path:

```sh
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

Windows release path:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\release.ps1
```

Output lands in:

```text
dist/deadgl-1.2.0/
dist/deadgl-1.2.0-source.zip
```

Warnings are errors. That is hygiene.
