# DEADGL v3.2.0

DGB proof parity cut.

## Added

- DGB parity checks
- DGB parity checks for:
  - `examples/near_clip.dgl`
  - `examples/command_machine.dgl`

## Changed

- Release path now runs DGB parity after golden proof and parser diagnostics.
- Release fails if `.dgl -> .dgb -> unpack -> hash` changes render output.

## Rule

```text
DGB envelope must not change framebuffer proof output.
```

## Run

```sh
make clean test
sh scripts/dgb_parity.sh
```
