# NIGHTSHIFT

This file is the unattended work order for DEADGL.

A cut is only real when it builds, tests, renders, hashes, and ships.

## Current stable

v10.0.0

## Next real cut

v10.0.1 suite hardening

## Release command

powershell -ExecutionPolicy Bypass -File .\scripts\cut.ps1 -Version 10.0.1 -Publish

## Hardening target

- README reflects the v10 command machine suite.
- PROOF reflects current golden hashes and suite artifacts.
- Release notes explain this is documentation/proof hardening, not a feature bump.

## Suite baseline

- deadgl: render, prove, hash, inspect, audit, shell, pack, scenepack, suite
- deadgl-inspect: standalone inspection
- deadpad: tiny scene editor
- deadview: native framebuffer viewer

## Queue

v10.0.1 suite hardening

## Working law

- no hidden render state
- no GPU wrapper
- no scene graph
- no asset-first trap
- no release without proof
