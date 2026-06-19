# DEADGL v3.2.0

DGB proof parity cut.

## Added

- `scripts/dgb_parity.ps1`
- DGB parity checks for:
  - `examples/near_clip.dgl`
  - `examples/command_machine.dgl`

## Changed

- Windows release path now runs DGB parity after golden proof and parser diagnostics.
- Release fails if `.dgl -> .dgb -> unpack -> hash` changes render output.

## Rule

```text
DGB envelope must not change framebuffer proof output.
```

## Run

```powershell
make clean test
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\dgb_parity.ps1
```
