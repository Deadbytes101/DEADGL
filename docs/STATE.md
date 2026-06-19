# State

Current cut: `1.8.0`

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
```

Next hard work:

```text
CLI scene inspection
```
