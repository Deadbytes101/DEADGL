# NIGHTSHIFT

This file is the unattended work order for DEADGL.

A cut is only real when it builds, tests, renders, hashes, and ships.

## Current stable

v11.0.0

## Next real cut

v11.0.1 language docs sync

## Release command

powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 11.0.1 -Publish

## Target

- README mentions the grammar command and `dgl.grammar` artifact.
- PROOF mentions the v11 grammar artifact.
- Release notes explain this is docs sync after v11.0.0.

## Queue

v11.0.1 language docs sync

## Working law

- no hidden render state
- no GPU wrapper
- no scene graph
- no asset-first trap
- no release without proof
