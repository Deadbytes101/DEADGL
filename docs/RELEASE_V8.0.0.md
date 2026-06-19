# DEADGL v8.0.0

Native viewer seed cut.

## Added

- `src/deadview.c`
- `deadview --version`
- `deadview image.ppm`
- P6 PPM metadata reader
- terminal ASCII preview output
- Makefile and CMake coverage for the viewer path

## Changed

- Windows release packages `deadview-windows.exe`
- Windows release packages `tiledemo.view`

## Rule

```text
The viewer starts native and dependency-free.
It reads DEADGL output directly.
No SDL.
No GLFW.
No GPU window wrapper yet.
```

## Run

```sh
./deadgl tiledemo -o tiledemo.ppm
./deadview tiledemo.ppm
```
