# DEADGL v2.0.0

CLI merge cut.

## Added

- `deadgl inspect scene.dgl`
- shared scene inspector core
- main CLI inspection path
- standalone `deadgl-inspect` remains available

## Changed

- version is 2.0.0
- `deadgl` now covers render, prove, hash, and inspect
- release package includes both `deadgl-windows.exe` and `deadgl-inspect-windows.exe`

## Run

```sh
./deadgl --version
./deadgl inspect examples/near_clip.dgl
./deadgl-inspect examples/near_clip.dgl
./deadgl run examples/command_machine.dgl -o command_machine.ppm
```
