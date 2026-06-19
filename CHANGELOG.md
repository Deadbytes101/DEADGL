# Changelog

## v2.0.0

CLI merge cut.

- added `deadgl inspect scene.dgl`
- kept standalone `deadgl-inspect`
- shared inspector core

## v1.9.0

Scene inspection tool cut.

- added `deadgl-inspect`
- added command counters
- added inspector docs

## v1.8.0

Proof scene cut.

- added `examples/near_clip.dgl`
- release package includes near clip proof output

## v1.7.0

Near-plane clipping cut.

- added triangle clipping at near plane
- added quad clipping at near plane
- clipped projected fill path
- clipped projected wire path

## v1.6.0

Near-plane guard cut.

- added `near`
- camera near plane can be set from `.dgl`

## v1.5.0

Flat light cut.

- added `light`
- added `shade flat`
- added `shade off`
- shaded projected face and mesh paths

## v1.4.0

Indexed mesh command cut.

- added `vertex`
- added mesh commands

## v1.3.0

Hidden-line geometry cut.

- added depth-tested wire lines
- added `quad`
- added `wirequad`
- added `cull on|off`

## v1.2.0

Geometry pipe cut.

- added `camera`
- added projected `face`
- added projected `wireface`
- added cube demo

## v1.1.0

Proof-machine cut.

- deterministic framebuffer hash
- proof file output
- command-machine example

## v1.0.0

Framebuffer cut.

- C99 software renderer core
- ARGB pixels
- depth buffer
- strict command parser
- PPM output
