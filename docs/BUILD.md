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

Local release path:

```sh
sh scripts/release.sh
```

Output lands in `dist/deadgl-11.0.0`.

Warnings are errors. That is hygiene.
