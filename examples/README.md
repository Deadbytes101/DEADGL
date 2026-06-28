# Examples

Run after `make`.

```sh
./build/deadgl demo cube -o cube.ppm
./build/deadgl run examples/shrine.dgl -o shrine.ppm
./build/deadgl run examples/depth.dgl -o depth.ppm
./build/deadgl run examples/tri.dgl -o tri.ppm
./build/deadgl run examples/cube.dgl -o cube_scene.ppm
./build/deadgl run examples/command_machine.dgl -o command_machine.ppm
./build/deadgl run examples/raster_truth.dgl -o raster_truth.ppm
./build/deadgl prove examples/command_machine.dgl -o command_machine.ppm -p command_machine.proof
./build/deadgl hash examples/command_machine.dgl
```

`command_machine.dgl` is the identity scene.

`cube.dgl` is the geometry smoke scene.

`raster_truth.dgl` is the clipping, depth, near-plane, and indexed mesh regression scene.
