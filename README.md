# DEADGL

Tiny CPU renderer. No OpenGL. No Vulkan. No driver ceremony.

DEADGL draws into memory and writes pixels back out. The pipe is small enough to read. The image is a byte array. The proof is a hash.

```sh
make test
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl run examples/shrine.dgl -o scene.ppm
./build/deadgl hash examples/shrine.dgl
```

## v1.0.0 iron cut

- C99, no dependency
- 32-bit ARGB framebuffer
- depth buffer
- clipped lines
- clipped rectangles
- circles
- filled circles
- wire triangles
- filled depth-tested triangles
- color interpolation across triangles
- deterministic framebuffer hash
- PPM output
- strict scene parser
- strict compiler flags
- unit tests
- sanitizer build
- Makefile, CMake, CI

## Not a costume

Not a game engine. Not a GL wrapper. Not a Vulkan shrine. Not a shader playground.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

## Build

```sh
make
make test
make sanitize
```

CMake:

```sh
cmake -S . -B build-cmake
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

## Commands

```sh
./build/deadgl --version
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl demo depth -o depth.ppm
./build/deadgl run examples/shrine.dgl -o scene.ppm
./build/deadgl hash examples/shrine.dgl
```

PPM is used because it is honest: header, width, height, raw RGB bytes.

## Scene

```text
canvas 640 360
clear #050608
grid 32 #171923
tri 320 36 96 304 544 304 #ffcc55
wiretri 320 36 96 304 544 304 #f0f0d8
fillcircle 320 180 22 #00ff99
circle 320 180 46 #e8e8d8
rect 40 32 560 296 #e8e8d8
line 0 180 639 180 #00ff99
```

## Rule

Keep the pipe visible. If a feature hides the pipe, it waits.
