# DEADGL v1.8.0

Proof scene cut.

## Added

- near clipping proof scene
- test path renders near clipping proof
- release package includes near clipping proof output

## Changed

- version is 1.8.0
- Makefile test path covers command machine and near clipping scene

## Run

```sh
./deadgl --version
./deadgl run examples/near_clip.dgl -o near_clip.ppm
./deadgl prove examples/near_clip.dgl -o near_clip.ppm -p near_clip.proof
./deadgl hash examples/near_clip.dgl
```
