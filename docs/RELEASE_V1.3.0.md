# DEADGL v1.3.0

Hidden-line geometry cut.

## Added

- depth-tested wire lines
- `quad`
- `wirequad`
- `cull on|off`
- cube demo uses quads
- command-machine scene uses quads

## Changed

- version is `1.3.0`
- cube wire path avoids the triangle diagonal
- projected wire tests against the depth buffer

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```
