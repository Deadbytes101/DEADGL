# DEADGL v3.0.0

Bytecode envelope cut.

## Added

- `deadgl pack scene.dgl -o scene.dgb`
- `deadgl unpack scene.dgb -o scene.dgl`
- DGB magic header: `DEADGL_DGB_1`
- Makefile parity test: pack then unpack must match the original scene text

## Changed

- Windows release packages `near_clip.dgb`
- Windows release packages unpacked parity text as `near_clip.unpack.dgl`

## Format

`v3.0.0` ships a conservative bytecode envelope first:

```text
DEADGL_DGB_1
raw dgl payload bytes
```

This creates a stable binary command stream boundary without changing renderer behavior.
Opcode-level encoding comes after this cut.

## Run

```sh
./deadgl pack examples/near_clip.dgl -o near_clip.dgb
./deadgl unpack near_clip.dgb -o near_clip.unpack.dgl
cmp examples/near_clip.dgl near_clip.unpack.dgl
```
