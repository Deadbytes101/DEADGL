# DEADGL v2.2.0

Golden proof test cut.

## Added

- `scripts/golden.ps1`
- fixed golden hash checks for:
  - cube demo
  - command_machine scene
  - near_clip scene

## Changed

- Windows release path now runs golden proof checks before packaging.
- Release fails if a core image hash moves unexpectedly.

## Golden hashes

```text
cube            95689452fd3b5017
command_machine 7dd8eb60b1510af7
near_clip       33dd8e01461d1f5c
```

## Run

```powershell
make clean test
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\golden.ps1
```
