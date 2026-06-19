# DEADGL v1.5.0

Flat light cut.

## Added

- light command
- shade flat command
- shade off command
- flat normal lighting for face paths

## Changed

- version is 1.5.0
- cube scene uses flat shade
- command-machine scene uses flat shade

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```
