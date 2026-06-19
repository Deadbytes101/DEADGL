# State

Current cut: `2.0.3`

The renderer is a CPU framebuffer command machine.

```text
.dgl -> parser -> framebuffer -> ppm -> proof hash
```

Working path:

```text
camera
near
near-plane clipping
near clipping proof scene
light
shade flat/off
face
wireface
quad
wirequad
vertex
meshtri
meshwiretri
meshquad
meshwirequad
cull on/off
depth-tested wire lines
depth buffer
PPM output
proof file
scene inspector tool
main CLI inspect command
```

CLI shape:

```text
deadgl render/prove/hash/inspect
deadgl-inspect standalone inspector
```

Next hard work:

```text
clean up CLI internals
```
