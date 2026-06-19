# DEADGL v1.4.0

Indexed mesh command cut.

## Added

- vertex command
- meshtri command
- meshwiretri command
- meshquad command
- meshwirequad command

## Changed

- version is 1.4.0
- cube scene uses indexed vertices
- command-machine scene uses indexed vertices

## Run

```sh
./deadgl --version
./deadgl demo cube -o cube.ppm
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./deadgl hash examples/command_machine.dgl
```
