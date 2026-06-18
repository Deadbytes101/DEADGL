# Proof format

`deadgl prove` renders a scene and writes a proof file.

```sh
./build/deadgl prove examples/shrine.dgl -o scene.ppm -p scene.proof
```

The file is plain text:

```text
DEADGL_PROOF
version 1.1.0
scene examples/shrine.dgl
output scene.ppm
width 640
height 360
pixels 230400
hash 0123456789abcdef
```

This is not a security signature. It is a reproducibility marker. Same renderer, same scene, same surface, same hash.
