# DEADGL v2.1.0

Source-contract split cut.

## Added

- `include/deadgl_cli.h`
- explicit CLI entry contract for the split command front
- v10 roadmap link in main docs

## Changed

- source layout begins moving away from hidden ad-hoc declarations
- release path stays local and deterministic

## Rules

```text
hashes must not move
commands must not change behavior
CLI must stay the same
```

## Run

```sh
./deadgl --version
./deadgl inspect examples/near_clip.dgl
./deadgl run examples/command_machine.dgl -o command_machine.ppm
./deadgl prove examples/near_clip.dgl -o near_clip.ppm -p near_clip.proof
```
