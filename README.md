# DEADGL

```text
DEADGL
CPU FRAMEBUFFER COMMAND MACHINE
INPUT:  .dgl command stream
OUTPUT: pixels + proof hash
GPU:    none
```

DEADGL is a small C99 renderer. It reads commands, writes pixels, saves PPM, and prints proof hashes.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

## Build

```sh
make clean test
make
```

## Run

```sh
./build/deadgl --version
./build/deadgl demo cube -o cube.ppm
./build/deadgl run examples/command_machine.dgl -o command_machine.ppm
./build/deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./build/deadgl hash examples/command_machine.dgl
```

Windows local release:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\cut.ps1 -Version 2.0.4 -Publish
```

## Machine

- C99
- no dependency
- ARGB framebuffer
- depth buffer
- lines, rectangles, circles, triangles
- projected 3D faces
- explicit camera command
- strict `.dgl` parser
- PPM output
- deterministic hash
- plain proof file
- local one-command release cutter

## Refuses

- no OBJ-first path
- no GPU wrapper
- no engine layer
- no scene graph
- no hidden render state

## Road to v10

See `docs/V10_ROADMAP.md`.

```text
v2.1  source split without hash movement
v3    bytecode command stream
v4    software console overlay
v5    tiny editor seed
v6    live shell
v7    tile renderer
v8    native viewer
v9    scene pack
v10   command machine suite
```

## Proof

```text
DEADGL_PROOF
version 2.0.3
scene examples/command_machine.dgl
output command_machine.ppm
width 640
height 360
pixels 230400
hash 0123456789abcdef
```

Keep the pipe visible. A command changes memory. The final image is bytes. The proof is the hash.
