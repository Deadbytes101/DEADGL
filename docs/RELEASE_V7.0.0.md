# DEADGL v7.0.0

Tile renderer seed cut.

## Added

- `src/deadgl_tile.c`
- `dgl_tile_grid(surface, tile, color_a, color_b)`
- `dgl_tile_frame(surface, tile, color)`
- `deadgl tiledemo -o tiledemo.ppm`
- Makefile and CMake coverage for tile traversal

## Changed

- Windows release packages `tiledemo.ppm`
- core tests verify tile traversal changes framebuffer hash
- static library now includes the tile traversal object

## Rule

```text
Tiles are traversal order over the framebuffer.
No GPU wrapper.
No scene graph.
No hidden render state.
```

## Run

```sh
./deadgl tiledemo -o tiledemo.ppm
```
