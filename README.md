# DEADGL

Tiny CPU renderer. No OpenGL. No Vulkan. No driver ceremony.

DEADGL draws into memory and writes pixels back out. The image is a byte array. The proof is a hash.

```sh
make test
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl run examples/shrine.dgl -o scene.ppm
./build/deadgl prove examples/shrine.dgl -o scene.ppm -p scene.proof
./build/deadgl hash examples/shrine.dgl
```

## v1.1.0

DEADGL is a graphics command machine.

- C99, no dependency
- ARGB framebuffer
- depth buffer
- clipped lines and rectangles
- circles and triangles
- deterministic framebuffer hash
- PPM output
- strict scene parser
- proof file output
- tests, sanitizer, Makefile, CMake, CI

## Not a costume

Not a game engine. Not a GL wrapper. Not an OBJ tutorial renderer.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

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

Keep the pipe visible.
