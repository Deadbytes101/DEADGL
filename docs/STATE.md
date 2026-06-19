# State

Current cut: `1.9.0`

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
```

Tool split:

```text
deadgl          render/prove/hash
deadgl-inspect  inspect .dgl without rendering
```

Next hard work:

```text
merge inspect into main CLI
```
