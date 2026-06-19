# Inspect

`deadgl-inspect` reads a `.dgl` command stream and prints counters without rendering.

```sh
deadgl-inspect --version
deadgl-inspect examples/near_clip.dgl
deadgl-inspect examples/command_machine.dgl
```

Output shape:

```text
DEADGL_INSPECT
version 1.9.0
scene examples/near_clip.dgl
lines 15
commands 15
canvas 1
camera 1
near 1
light 1
shade 1
cull 1
vertex 0
mesh 0
primitive 9
unknown 0
```

Use it before render when a scene feels wrong.
