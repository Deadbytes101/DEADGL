# State

Current cut: `1.2.0`

The renderer is a CPU framebuffer command machine.

```text
.dgl -> parser -> framebuffer -> ppm -> proof hash
```

Working path:

```text
camera
face
wireface
depth buffer
PPM output
proof file
```

Known proof points:

```text
cube demo        7011797b3adabb5c
command_machine 26a2539edae4cc96
```

Next hard work is renderer work, not repo decoration:

```text
depth-tested wire lines
backface culling
quad command
indexed mesh command
flat light
```
