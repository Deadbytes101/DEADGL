# Changelog

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
- cube scene sets explicit near plane
- command machine scene sets explicit near plane

## v1.5.0

Flat light cut.

- added `light`
- added `shade flat`
- added `shade off`
- shaded projected face and mesh paths
- cube scene now uses flat shade
- command machine scene now uses flat shade

## v1.4.0

Indexed mesh command cut.

- added `vertex`
- added `meshtri`
- added `meshwiretri`
- added `meshquad`
- added `meshwirequad`
- cube scene now uses indexed vertices
- command machine scene now uses indexed vertices

## v1.3.0

Hidden-line geometry cut.

- added depth-tested wire lines
- added `quad`
- added `wirequad`
- added `cull on|off`
- cube demo now uses quads
- command machine scene now uses quads

## v1.2.0

Geometry pipe cut.

- added `camera`
- added projected `face`
- added projected `wireface`
- added cube demo
- moved command machine example to projected geometry
- release scripts now cut `deadgl-1.2.0`
- proof record updated for v1.2 hashes

## v1.1.0

Proof-machine cut.

- deterministic framebuffer hash
- proof file output
- command-machine example
- Windows release artifact path

## v1.0.0

Framebuffer cut.

- C99 software renderer core
- ARGB pixels
- depth buffer
- lines, rectangles, circles, triangles
- strict command parser
- PPM output
