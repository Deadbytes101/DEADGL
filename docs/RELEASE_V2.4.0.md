# DEADGL v2.4.0

Deterministic audit command cut.

## Added

- `deadgl audit scene.dgl`
- audit route through the deterministic inspector core
- Makefile test coverage for the audit command path

## Changed

- `make clean test` now writes `build/command_machine.audit`
- release builds keep the same renderer and proof path

## Rule

```text
Audit reads command structure. It does not render. It does not mutate framebuffer memory.
```

## Run

```sh
./deadgl audit examples/command_machine.dgl
make clean test
```
