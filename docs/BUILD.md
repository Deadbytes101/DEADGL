# Build

Normal:

```sh
make clean test
```

Harder:

```sh
make clean sanitize
```

CMake:

```sh
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

Warnings are errors. That is not a flex. It is hygiene.
