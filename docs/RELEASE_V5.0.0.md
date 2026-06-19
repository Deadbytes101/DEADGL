# DEADGL v5.0.0

DEADPAD seed cut.

## Added

- `deadpad --version`
- `deadpad new scene.dgl`
- `deadpad cat scene.dgl`
- `deadpad append scene.dgl COMMAND...`
- Makefile test coverage for scene creation, append, numbered cat, and render

## Changed

- Windows release packages `deadpad-windows.exe`
- Windows release packages `deadpad_seed.dgl`
- Windows release packages `deadpad_seed.cat`
- Windows release packages `deadpad_seed.ppm`

## Rule

```text
The editor starts as a tool, not a framework.
It writes plain .dgl text.
DEADGL renders the result.
```

## Run

```sh
./deadpad new scene.dgl
./deadpad append scene.dgl line 8 8 120 8 0xff8822
./deadpad cat scene.dgl
./deadgl run scene.dgl -o scene.ppm
```
