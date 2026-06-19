# State

Current cut: `1.5.0`

The renderer is a CPU framebuffer command machine.

```text
.dgl -> parser -> framebuffer -> ppm -> proof hash
```

Working path:

```text
camera
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
near-plane clipping
```
