# Release notes

## v1.1.0 proof-machine cut

This cut makes DEADGL less like a normal tiny 3D renderer and more like its own machine.

- bumped public version to 1.1.0
- added `deadgl prove scene.dgl -o out.ppm -p out.proof`
- proof file records version, scene, output, size, pixel count, and framebuffer hash
- Makefile test path now exercises proof output
- README now positions DEADGL as a graphics command machine
- added docs for identity, proof format, and command-machine direction

## v1.0.0 iron cut

- hardened surface allocation
- explicit result codes
- line clipping before Bresenham
- overflow-safe rectangle clipping
- strict color parser
- strict scene parser
- triangle color interpolation
- deterministic framebuffer hash
- broader tests
- sanitizer path
- docs rewritten short and plain
