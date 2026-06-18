# DEADGL v1.1.0

CPU framebuffer command machine.

Input is `.dgl`. Output is pixels. Proof is a hash.

## Ships

- `deadgl-windows.exe` built by the local release script
- `deadgl-1.1.0-source.zip`
- command machine PPM output
- command machine proof file
- benchmark report
- SHA256 sums

## Local validation

- `make clean test` passed
- sanitizer path probed the toolchain; w64devkit may skip ASan/UBSan when runtimes are unavailable
- local release script produced binary, proof, benchmark, source zip, and checksums

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
