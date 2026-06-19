# DEADGL v6.0.1

Shell packaging repair cut.

## Fixed

- Release packaging now renders `shell.ppm` from `shell.stdin.dgl` through the checked shell file path.
- Release packaging now checks native command exit codes for shell, DGB, audit, hash, inspect, and proof artifacts.
- A shell artifact failure can no longer be hidden behind a successful tag/release.

## Context

`v6.0.0` shipped the shell feature and passed Makefile shell tests, but the release packaging step printed a shell parser error while continuing. This repair cut closes that release-script hole.

## Rule

```text
No release artifact without checked command exit status.
```
