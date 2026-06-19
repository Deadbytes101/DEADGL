# DEADGL v1.7.0

Near-plane clipping cut.

## Added

- triangle clipping at near plane
- quad clipping at near plane
- clipped fill path
- clipped wire path

## Changed

- version is 1.7.0
- face paths clip before projection

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```
