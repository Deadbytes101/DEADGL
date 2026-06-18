# Scene format

A `.dgl` file is a command stream.

```text
canvas W H
clear COLOR
grid STEP COLOR
line X0 Y0 X1 Y1 COLOR
rect X Y W H COLOR
fillrect X Y W H COLOR
circle X Y R COLOR
fillcircle X Y R COLOR
tri X0 Y0 X1 Y1 X2 Y2 COLOR
wiretri X0 Y0 X1 Y1 X2 Y2 COLOR
ztri X0 Y0 Z0 X1 Y1 Z1 X2 Y2 Z2 COLOR
```

Colors accept `#RRGGBB`, `0xRRGGBB`, or `0xAARRGGBB`.
