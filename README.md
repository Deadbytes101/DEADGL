# DEADGL

```text
DEADGL
CPU FRAMEBUFFER COMMAND MACHINE
INPUT:  .dgl command stream
OUTPUT: pixels + proof hash
GPU:    none
```

DEADGL is not a tiny OBJ renderer course. It is not a game engine. It is not a wrapper.

It is a small C99 machine that takes drawing commands, writes a framebuffer, emits a PPM image, and can produce a plain proof file with the framebuffer hash.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

## Run it

```sh
make test
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl run examples/command_machine.dgl -o command_machine.ppm
./build/deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./build/deadgl hash examples/command_machine.dgl
```

## What it is

- C99, no dependency
- ARGB framebuffer
- depth buffer
- clipped lines and rectangles
- circles and triangles
- strict `.dgl` parser
- deterministic framebuffer hash
- PPM output
- proof file output
- unit tests
- sanitizer target
- Makefile, CMake, CI

## What it refuses

- no OBJ-first path
- no lesson ladder
- no hidden engine layer
- no GPU cosplay
- no magic scene graph

## Proof file

```text
DEADGL_PROOF
version 1.1.0
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash 0123456789abcdef
```

## Commands

```sh
./build/deadgl --version
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl demo depth -o depth.ppm
./build/deadgl run examples/shrine.dgl -o scene.ppm
./build/deadgl prove examples/shrine.dgl -o scene.ppm -p scene.proof
./build/deadgl hash examples/shrine.dgl
```

## Rule

Keep the pipe visible. A command changes memory now. The final image is bytes. The proof is the hash.
