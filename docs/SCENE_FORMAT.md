# Scene format

A `.dgl` file is a command stream. One command per line.

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
camera EX EY EZ TX TY TZ FOV
cull on
cull off
face X0 Y0 Z0 X1 Y1 Z1 X2 Y2 Z2 COLOR
wireface X0 Y0 Z0 X1 Y1 Z1 X2 Y2 Z2 COLOR
quad X0 Y0 Z0 X1 Y1 Z1 X2 Y2 Z2 X3 Y3 Z3 COLOR
wirequad X0 Y0 Z0 X1 Y1 Z1 X2 Y2 Z2 X3 Y3 Z3 COLOR
```

Unknown commands are errors.
