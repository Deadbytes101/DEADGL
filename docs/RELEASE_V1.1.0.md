# DEADGL v1.1.0

CPU framebuffer command machine.

Input is `.dgl`. Output is pixels. Proof is a hash.

## Ships

- `deadgl` Linux binary from GitHub Actions
- source tarball
- command machine PPM output
- command machine proof file
- benchmark report from Actions

## Core

- C99 renderer
- ARGB framebuffer
- depth buffer
- clipped lines and rectangles
- circles and triangles
- strict scene parser
- deterministic hash
- proof file output

## Commands

```sh
./deadgl --version
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```

No OBJ course. No engine layer. Commands in. Memory changes. Hash out.
