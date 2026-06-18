# Proof

v1.1.0 proof-machine cut.

Validation commands:

```sh
make clean test
make clean sanitize
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

The Makefile path renders demos, renders a scene, writes a proof file, hashes the scene, and checks that outputs exist.

Expected version:

```text
DEADGL 1.1.0
```

Proof file shape:

```text
DEADGL_PROOF
version 1.1.0
scene examples/shrine.dgl
output build/proof.ppm
width 640
height 360
pixels 230400
hash <framebuffer-hash>
```
