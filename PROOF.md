# Proof

v1.3.0 hidden-line geometry cut.

DEADGL proves the final framebuffer, not a scene graph and not a driver path.

```text
command -> memory -> image -> hash
```

## Validation

```sh
make clean test
make clean sanitize
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

Windows validation from the v1.3 cut:

```text
PASS deadgl core
DEADGL 1.3.0
cube demo hash f79e4f92d2331f42
command_machine hash b1d6711058de7832
```

## Proof file shape

```text
DEADGL_PROOF
version 1.3.0
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash b1d6711058de7832
```

If the same command stream and code produce a different hash, something changed. That is the point.
