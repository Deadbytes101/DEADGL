# Proof

DEADGL proves the final framebuffer, not a scene graph and not a driver path.

```text
command -> memory -> image -> hash
```

## Current proof line

v11.0.0 shipped the language hardening cut with `dgl.grammar` as a release artifact.

```text
GOLDEN cube 95689452fd3b5017
GOLDEN command_machine 7dd8eb60b1510af7
GOLDEN near_clip 33dd8e01461d1f5c
DGB PARITY near_clip 33dd8e01461d1f5c
DGB PARITY command_machine 7dd8eb60b1510af7
```

## Validation

```sh
make clean test
make clean sanitize
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

Windows release validation:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 11.0.1 -Publish
```

## Proof file shape

```text
DEADGL_PROOF
version 11.0.0
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash 7dd8eb60b1510af7
```

## Suite proof artifacts

```text
dgl.grammar
command_machine.proof
command_machine.hash
command_machine.suite
command_machine.suite.ppm
command_machine.dgp
command_machine.unpack.dgl
command_machine.unpack.ppm
near_clip.proof
near_clip.hash
near_clip.dgb
near_clip.unpack.dgl
SHA256SUMS.txt
```

If the same command stream and code produce a different hash, something changed. That is the point.
