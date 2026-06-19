# DEADGL v2.3.0

Parser diagnostics probe cut.

## Added

- `scripts/parser_probe.ps1`
- failing scene probes for:
  - unknown command
  - bad arity
  - bad integer
  - bad float
  - bad color
  - bad vertex index

## Changed

- Windows release path now runs parser diagnostics before packaging.
- Release fails if expected parser error text disappears.

## Run

```powershell
make clean test
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\golden.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\parser_probe.ps1
```
