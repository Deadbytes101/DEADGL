# API

The public API is `include/deadgl.h`.

```c
DGL_Surface s;
dgl_surface_init(&s, 640, 360);
dgl_clear(&s, DGL_RGB(5, 6, 8));
dgl_line(&s, 0, 0, 639, 359, DGL_RGB(0, 255, 153));
dgl_save_ppm(&s, "out.ppm");
dgl_surface_free(&s);
```

Draw calls clip or ignore bad coordinates. Allocation and file output return status codes.
