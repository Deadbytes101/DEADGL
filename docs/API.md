# API

The public surface is `include/deadgl.h`.

```c
DGL_Surface s = {0};
dgl_surface_init(&s, 640, 360);
dgl_clear(&s, DGL_RGB(5, 6, 8));
dgl_line(&s, 0, 0, 639, 359, DGL_RGB(0, 255, 153));
dgl_save_ppm(&s, "out.ppm");
dgl_surface_free(&s);
```

Return codes are plain integers. Bad draw coordinates clip or do nothing. Allocation and file output report failure.

The framebuffer is `uint32_t ARGB`. Depth is `float`. Nothing is retained. A draw call changes memory now.
