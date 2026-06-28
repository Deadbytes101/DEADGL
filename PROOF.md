# Proof

DEADGL proves the final framebuffer, not a scene graph and not a driver path.

```text
command -> memory -> image -> hash
```

## Current proof line

v11.0.0 shipped the language hardening cut with `dgl.grammar` as a release artifact.

```text
GOLDEN cube d710810073eeec70
GOLDEN command_machine ddcf9658d3e00f08
GOLDEN near_clip c84cf0589c219386
DGB PARITY near_clip c84cf0589c219386
DGB PARITY command_machine ddcf9658d3e00f08
```

## Validation

```sh
make clean test
make clean sanitize
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
sh scripts/verify_render_truth.sh
sh scripts/golden.sh
sh scripts/dgb_parity.sh
sh scripts/release.sh
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
hash ddcf9658d3e00f08
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
