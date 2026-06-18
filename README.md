# DEADGL

Tiny CPU renderer. No OpenGL. No Vulkan. No driver ceremony.

DEADGL draws into memory and writes pixels back out. It is a small C99 renderer, a command-line tool, and a plain text scene runner. The point is not to replace industrial graphics APIs. The point is to keep the whole pipe visible.

```sh
make test
./build/deadgl demo shrine -o shrine.ppm
./build/deadgl run examples/shrine.dgl -o scene.ppm
./build/deadgl hash examples/shrine.dgl
```

## What is in v0.3.0

- 32-bit ARGB framebuffer
- depth buffer
- pixel, line, rect, filled rect
- circle and filled circle
- wire triangle and filled triangle
- deterministic framebuffer hash
- PPM image output
- tiny `.dgl` scene format
- strict C build
- unit tests and sanitizer target
- Makefile, CMake, GitHub Actions

## What it is not

Not a game engine. Not a GL wrapper. Not a Vulkan costume. Not a shader toy.

OpenGL and Vulkan talk to GPUs. DEADGL talks to memory.

## Build

```sh
make
make test
make sanitize
```

CMake path:

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

PPM is used because it is simple: header, width, height, raw RGB bytes.

## Scene file

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

If the feature makes the pipe harder to see, it waits.
