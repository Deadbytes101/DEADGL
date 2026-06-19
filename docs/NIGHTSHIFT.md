# NIGHTSHIFT

This file is the unattended work order for DEADGL.

The target is v10, but versions are not allowed to be fake. A cut is only real when it builds, tests, renders, hashes, and ships.

## Current stable

```text
v8.0.0
```

## Next real cut

```text
v9.0.0 scene pack
```

## Release command

```powershell
git pull
powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 9.0.0 -Publish
```

## No fake v10 rule

Do not tag v10 just to reach a number.

v10 means the command machine suite exists:

```text
deadgl          render/prove/hash/inspect/audit/shell
deadgl-inspect  standalone inspection
deadpad         tiny scene editor
deadview        native framebuffer viewer
```

## Queue

```text
9.0.0  scene pack
10.0.0 command machine suite
```

## Working law

```text
No hidden render state.
No GPU wrapper.
No scene graph.
No asset-first trap.
No release without proof.
```
