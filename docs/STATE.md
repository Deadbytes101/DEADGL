# State

Current cut: `9.0.0`

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
local one-command release cutter
```

CLI shape:

```text
deadgl render/prove/hash/inspect
deadgl-inspect standalone inspector
```

Road:

```text
v2.1  source split without hash movement
v3    bytecode command stream
v4    software console overlay
v5    tiny editor seed
v6    live shell
v7    tile renderer
v8    native viewer
v9    scene pack
v10   command machine suite
```

Next hard work:

```text
split deadgl_cli.c into small C units without changing output hashes
```
