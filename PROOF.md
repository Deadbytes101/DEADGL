# Proof

v1.2.0 geometry pipe cut.

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

Windows release path:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\release.ps1
```

## Expected version

```text
DEADGL 1.2.0
```

## Known v1.2 hashes

These are proof points from the v1.2.0 cut.

```text
shrine demo          5c080b02c2ed03cb
depth demo           1341cd43556e0a08
cube demo            7011797b3adabb5c
command_machine      26a2539edae4cc96
```

## Proof file shape

```text
DEADGL_PROOF
version 1.2.0
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash 26a2539edae4cc96
```

If the same command stream and code produce a different hash, something changed. That is the point.
