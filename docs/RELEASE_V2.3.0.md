# DEADGL v2.3.0

Parser diagnostics probe cut.

## Added

- failing scene probes for:
  - unknown command
  - bad arity
  - bad integer
  - bad float
  - bad color
  - bad vertex index

## Changed

- Release path now runs parser diagnostics before packaging.
- Release fails if expected parser error text disappears.

## Run

```sh
make clean test
```
