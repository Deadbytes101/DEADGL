# DEADGL v1.2.0

Geometry pipe cut.

## Added

- explicit camera command
- projected `face`
- projected `wireface`
- cube demo
- projected command-machine example

## Still true

- C99
- no dependency
- CPU framebuffer
- depth buffer
- PPM output
- proof hash
- strict parser

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```

No OBJ loader. No GPU backend. No scene graph.
