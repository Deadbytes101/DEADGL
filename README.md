# DEADGL

```text
DEADGL
CPU FRAMEBUFFER COMMAND MACHINE
INPUT:  .dgl command stream
OUTPUT: pixels + proof hash
GPU:    none
```

DEADGL is a C99 command-machine renderer. It reads plain text `.dgl` commands, writes an ARGB framebuffer in CPU memory, saves PPM images, and emits proof hashes.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

## Build

```sh
make clean test
make
```

## Core commands

```sh
./build/deadgl --version
./build/deadgl demo cube -o cube.ppm
./build/deadgl run examples/command_machine.dgl -o command_machine.ppm
./build/deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./build/deadgl hash examples/command_machine.dgl
./build/deadgl inspect examples/near_clip.dgl
./build/deadgl audit examples/command_machine.dgl
```

## Suite tools

```text
deadgl          render / prove / hash / inspect / audit / shell / pack / scenepack / suite
deadgl-inspect  standalone scene inspection
deadpad         plain-text scene editor seed
deadview        native PPM viewer seed
```

## Suite run

```sh
./build/deadpad new scene.dgl
./build/deadpad append scene.dgl line 8 8 120 8 0xff8822
./build/deadgl run scene.dgl -o scene.ppm
./build/deadview scene.ppm
./build/deadgl suite examples/command_machine.dgl -o command_machine.suite
```

## Release

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 10.0.1 -Publish
```

The release cutter must build, test, render, hash, package proof artifacts, tag, and publish. If any checked step fails, the cut is not real.

## Machine

- C99
- no dependency
- ARGB framebuffer
- depth buffer
- lines, rectangles, circles, triangles
- projected 3D faces
- explicit camera command
- strict `.dgl` parser
- DGB bytecode envelope
- DGP scene-pack envelope
- shell command stream
- tile traversal seed
- terminal PPM viewer seed
- deterministic hash
- plain proof file
- local one-command release cutter

## Refuses

- no OBJ-first path
- no GPU wrapper
- no engine layer
- no scene graph
- no hidden render state

## Proof shape

```text
DEADGL_PROOF
version 10.0.0
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash 7dd8eb60b1510af7
```

Keep the pipe visible. A command changes memory. The final image is bytes. The proof is the hash.
