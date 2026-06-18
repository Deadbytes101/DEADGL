# DEADGL v1.2.0

Geometry pipe cut.

Input is still `.dgl`. Output is still pixels. Proof is still a hash.

## Adds

- explicit `camera EX EY EZ TX TY TZ FOV`
- projected 3D `face`
- projected 3D `wireface`
- cube demo
- projected cube scene
- stricter float parsing for geometry commands

## Core

- C99 renderer
- ARGB framebuffer
- depth buffer
- clipped lines and rectangles
- circles and triangles
- projected 3D triangles
- strict scene parser
- deterministic hash
- proof file output

## Commands

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/cube.dgl -o cube_scene.ppm
./deadgl prove examples/cube.dgl -o cube_scene.ppm -p cube_scene.proof
./deadgl hash examples/cube.dgl
```

No OBJ loader yet. No engine layer. Command stream in. CPU projection. Pixels out.
