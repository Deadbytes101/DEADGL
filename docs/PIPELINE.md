# Pipeline

```text
API or .dgl command
        |
        v
raster operation
        |
        v
ARGB framebuffer + depth buffer
        |
        +--> PPM
        +--> FNV-1a hash
```

There is no retained scene graph. There is no command buffer. The framebuffer is the truth surface.

Lines use clipping before Bresenham. Rectangles clip before filling. Triangles use edge functions and a depth test. The hash walks the pixels in memory order.
