# DEADGL v9.0.0

Scene pack cut.

## Added

- `deadgl scenepack scene.dgl -o scene.dgp`
- `deadgl sceneunpack scene.dgp -o scene.dgl`
- `DEADGL_DGP_1` scene pack envelope
- Makefile coverage for pack, unpack, byte compare, and render from unpacked scene

## Changed

- Windows release packages `command_machine.dgp`
- Windows release packages `command_machine.unpack.dgl`
- Windows release packages `command_machine.unpack.ppm`

## Rule

```text
Scene pack is a scene envelope, not an asset framework.
The unpacked scene must be byte-identical to the original .dgl payload.
DEADGL renders the unpacked scene through the normal renderer.
```

## Run

```sh
./deadgl scenepack examples/command_machine.dgl -o command_machine.dgp
./deadgl sceneunpack command_machine.dgp -o command_machine.unpack.dgl
./deadgl run command_machine.unpack.dgl -o command_machine.unpack.ppm
```
