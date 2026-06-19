# DGL language

DGL is a plain text scene format for DEADGL.

## Values

- color: `#RRGGBB` or `0xRRGGBB`
- int: signed decimal integer
- float: signed decimal float

## Common lines

```text
canvas WIDTH HEIGHT
clear COLOR
camera X Y Z FOV
grid STEP COLOR
pixel X Y COLOR
line X0 Y0 X1 Y1 COLOR
rect X Y W H COLOR
fillrect X Y W H COLOR
circle CX CY R COLOR
fillcircle CX CY R COLOR
tri X0 Y0 X1 Y1 X2 Y2 COLOR
meshtri I0 I1 I2 COLOR
text X Y COLOR WORDS...
```

## Files

- `.dgl`: plain scene text
- `.dgb`: bytecode envelope
- `.dgp`: scene envelope

## Tooling

```text
deadgl grammar
deadgl inspect examples/near_clip.dgl
deadgl audit examples/command_machine.dgl
```
