# DEADGL v1.6.0

Near-plane guard cut.

## Added

- near command
- explicit camera near plane control
- projected geometry rejects vertices behind the near plane

## Changed

- version is 1.6.0
- cube scene sets near 0.05
- command-machine scene sets near 0.05

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```
