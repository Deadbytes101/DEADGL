# DEADGL v4.0.0

Software console overlay cut.

## Added

- `dgl_char(surface, x, y, color, ch)`
- `dgl_text(surface, x, y, color, text)`
- `deadgl textdemo -o textdemo.ppm`
- Makefile test coverage for software text output

## Changed

- Windows release packages `textdemo.ppm`
- core tests verify text drawing changes framebuffer hash

## Rule

```text
Text is pixels. No font library. No GPU. No hidden UI layer.
```

## Run

```sh
./deadgl textdemo -o textdemo.ppm
```
