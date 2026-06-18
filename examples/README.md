# Examples

Run these after `make`.

```sh
./build/deadgl run examples/shrine.dgl -o shrine.ppm
./build/deadgl run examples/depth.dgl -o depth.ppm
./build/deadgl run examples/tri.dgl -o tri.ppm
./build/deadgl run examples/command_machine.dgl -o command_machine.ppm
./build/deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
```

`command_machine.dgl` is the identity example. It is command stream art: lines, circles, triangles, depth, proof.
