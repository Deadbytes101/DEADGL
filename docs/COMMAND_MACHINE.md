# Command machine

DEADGL scene files are not assets. They are programs for a tiny drawing machine.

A line draws. A triangle fills. A hash proves the final memory image.

The format stays boring so the renderer stays inspectable.

```text
canvas 640 360
clear #050608
line 0 0 639 359 #00ff99
tri 320 36 96 304 544 304 #ffcc55
```

The future path is command-machine work: macros, include files, fixed-point mode, palette mode, replay logs. Not OBJ loading first.
