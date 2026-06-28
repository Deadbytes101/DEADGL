# DEADGL v10.0.1

Suite hardening cut.

## Changed

- README now describes the shipped v10 command machine suite.
- PROOF now records current golden hashes, DGB parity hashes, and suite proof artifacts.
- NIGHTSHIFT now points at the v10.0.1 hardening cut.

## No feature bump

This is not a new feature release. It hardens public documentation and proof language after the v10.0.0 command machine suite cut.

## Rule

```text
A public suite needs public proof text.
No hidden render state.
No GPU wrapper.
No scene graph.
No release without proof.
```

## Run

```sh
sh scripts/release.sh
```
