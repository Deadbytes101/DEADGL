# Proof

Validated before pushing this cut:

```text
make clean test
PASS deadgl core
DEADGL 0.3.0
hash 4099cba387116462
hash af46738a4ba7ca23
f07b996be26916c8  examples/shrine.dgl
```

```text
make sanitize
PASS deadgl core
DEADGL 0.3.0
```

```text
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
100% tests passed, 0 tests failed out of 4
```
