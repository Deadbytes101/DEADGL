# DEADGL v1.9.0

Scene inspection tool cut.

## Added

- `deadgl-inspect` tool
- command counting
- vertex and mesh counting
- camera, near, light, shade, and cull counters
- Makefile and CMake test coverage for scene inspection

## Changed

- version is 1.9.0
- release package includes `deadgl-inspect`

## Run

```sh
./deadgl-inspect --version
./deadgl-inspect examples/near_clip.dgl
./deadgl-inspect examples/command_machine.dgl
```
