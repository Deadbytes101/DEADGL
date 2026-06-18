# Proof

Generated for v1.0.0 before pushing.

```text
make clean test
PASS deadgl core
DEADGL 1.0.0
hash 5c080b02c2ed03cb
hash 1341cd43556e0a08
b452b48c23800b9d  examples/shrine.dgl
```

```text
make clean sanitize
PASS deadgl core
DEADGL 1.0.0
hash 5c080b02c2ed03cb
hash 1341cd43556e0a08
b452b48c23800b9d  examples/shrine.dgl
```

```text
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
100% tests passed, 0 tests failed out of 3
```
