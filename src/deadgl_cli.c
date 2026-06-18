#include "deadgl.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t color_of(const char *s) {
    char *end;
    unsigned long v;
    if (s == NULL) {
        return DGL_RGB(255, 0, 255);
    }
    if (s[0] == '#') {
        s++;
    } else if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }
    errno = 0;
    v = strtoul(s, &end, 16);
    if (errno != 0 || end == s) {
        return DGL_RGB(255, 0, 255);
    }
    if (v <= 0xfffffful) {
        v |= 0xff000000ul;
    }
    return (uint32_t)v;
}

static void draw_grid(DGL_Surface *s, int step, uint32_t color) {
    int x;
    int y;
    if (step <= 0) {
        return;
    }
    for (x = 0; x < s->width; x += step) {
        dgl_line(s, x, 0, x, s->height - 1, color);
    }
    for (y = 0; y < s->height; y += step) {
        dgl_line(s, 0, y, s->width - 1, y, color);
    }
}

static int reset_surface(DGL_Surface *s, int w, int h) {
    dgl_surface_free(s);
    return dgl_surface_init(s, w, h);
}

static int run_scene(const char *path, const char *out, int hash_only) {
    FILE *f;
    char line[512];
    DGL_Surface s;
    int line_no = 0;
    if (dgl_surface_init(&s, 640, 360) != DGL_OK) {
        return 1;
    }
    dgl_clear(&s, DGL_RGB(0, 0, 0));
    f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "deadgl: cannot open %s\n", path);
        dgl_surface_free(&s);
        return 1;
    }
    while (fgets(line, sizeof(line), f) != NULL) {
        char cmd[32];
        line_no++;
        if (line[0] == ';' || line[0] == '\n' || (line[0] == '/' && line[1] == '/')) {
            continue;
        }
        if (sscanf(line, "%31s", cmd) != 1) {
            continue;
        }
        if (strcmp(cmd, "canvas") == 0) {
            int w;
            int h;
            if (sscanf(line, "%*s %d %d", &w, &h) == 2 && reset_surface(&s, w, h) == DGL_OK) {
                dgl_clear(&s, DGL_RGB(0, 0, 0));
            } else {
                fprintf(stderr, "deadgl: bad canvas at line %d\n", line_no);
                fclose(f);
                dgl_surface_free(&s);
                return 1;
            }
        } else if (strcmp(cmd, "clear") == 0) {
            char c[32];
            if (sscanf(line, "%*s %31s", c) == 1) {
                dgl_clear(&s, color_of(c));
                dgl_clear_depth(&s, DGL_FAR);
            }
        } else if (strcmp(cmd, "line") == 0) {
            int x0, y0, x1, y1;
            char c[32];
            if (sscanf(line, "%*s %d %d %d %d %31s", &x0, &y0, &x1, &y1, c) == 5) {
                dgl_line(&s, x0, y0, x1, y1, color_of(c));
            }
        } else if (strcmp(cmd, "rect") == 0 || strcmp(cmd, "fillrect") == 0) {
            int x, y, w, h;
            char c[32];
            if (sscanf(line, "%*s %d %d %d %d %31s", &x, &y, &w, &h, c) == 5) {
                if (strcmp(cmd, "rect") == 0) {
                    dgl_rect(&s, x, y, w, h, color_of(c));
                } else {
                    dgl_fill_rect(&s, x, y, w, h, color_of(c));
                }
            }
        } else if (strcmp(cmd, "circle") == 0 || strcmp(cmd, "fillcircle") == 0) {
            int x, y, r;
            char c[32];
            if (sscanf(line, "%*s %d %d %d %31s", &x, &y, &r, c) == 4) {
                if (strcmp(cmd, "circle") == 0) {
                    dgl_circle(&s, x, y, r, color_of(c));
                } else {
                    dgl_fill_circle(&s, x, y, r, color_of(c));
                }
            }
        } else if (strcmp(cmd, "tri") == 0 || strcmp(cmd, "wiretri") == 0) {
            DGL_Vertex a;
            DGL_Vertex b;
            DGL_Vertex c;
            char col[32];
            if (sscanf(line, "%*s %f %f %f %f %f %f %31s", &a.x, &a.y, &b.x, &b.y, &c.x, &c.y, col) == 7) {
                a.z = 0.5f;
                b.z = 0.5f;
                c.z = 0.5f;
                a.color = color_of(col);
                b.color = a.color;
                c.color = a.color;
                if (strcmp(cmd, "tri") == 0) {
                    dgl_tri(&s, a, b, c);
                } else {
                    dgl_wire_tri(&s, a, b, c, a.color);
                }
            }
        } else if (strcmp(cmd, "ztri") == 0) {
            DGL_Vertex a;
            DGL_Vertex b;
            DGL_Vertex c;
            char col[32];
            if (sscanf(line, "%*s %f %f %f %f %f %f %f %f %f %31s", &a.x, &a.y, &a.z, &b.x, &b.y, &b.z, &c.x, &c.y, &c.z, col) == 10) {
                a.color = color_of(col);
                b.color = a.color;
                c.color = a.color;
                dgl_tri(&s, a, b, c);
            }
        } else if (strcmp(cmd, "grid") == 0) {
            int step;
            char c[32];
            if (sscanf(line, "%*s %d %31s", &step, c) == 2) {
                draw_grid(&s, step, color_of(c));
            }
        }
    }
    fclose(f);
    if (hash_only) {
        printf("%016llx  %s\n", (unsigned long long)dgl_hash(&s), path);
    } else if (dgl_save_ppm(&s, out) != DGL_OK) {
        fprintf(stderr, "deadgl: cannot write %s\n", out);
        dgl_surface_free(&s);
        return 1;
    }
    dgl_surface_free(&s);
    return 0;
}

static int demo(const char *name, const char *out) {
    DGL_Surface s;
    DGL_Vertex a;
    DGL_Vertex b;
    DGL_Vertex c;
    if (dgl_surface_init(&s, 640, 360) != DGL_OK) {
        return 1;
    }
    dgl_clear(&s, DGL_RGB(5, 6, 8));
    dgl_clear_depth(&s, DGL_FAR);
    draw_grid(&s, 32, DGL_RGB(20, 24, 32));
    if (strcmp(name, "depth") == 0) {
        a = (DGL_Vertex){120.0f, 60.0f, 0.8f, DGL_RGB(120, 50, 255)};
        b = (DGL_Vertex){560.0f, 300.0f, 0.8f, DGL_RGB(120, 50, 255)};
        c = (DGL_Vertex){80.0f, 310.0f, 0.8f, DGL_RGB(120, 50, 255)};
        dgl_tri(&s, a, b, c);
        a = (DGL_Vertex){520.0f, 50.0f, 0.2f, DGL_RGB(255, 180, 50)};
        b = (DGL_Vertex){560.0f, 310.0f, 0.2f, DGL_RGB(255, 180, 50)};
        c = (DGL_Vertex){160.0f, 260.0f, 0.2f, DGL_RGB(255, 180, 50)};
        dgl_tri(&s, a, b, c);
    } else {
        a = (DGL_Vertex){320.0f, 36.0f, 0.5f, DGL_RGB(255, 204, 85)};
        b = (DGL_Vertex){96.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)};
        c = (DGL_Vertex){544.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)};
        dgl_tri(&s, a, b, c);
        dgl_wire_tri(&s, a, b, c, DGL_RGB(240, 240, 216));
        dgl_fill_circle(&s, 320, 180, 22, DGL_RGB(0, 255, 153));
        dgl_circle(&s, 320, 180, 46, DGL_RGB(232, 232, 216));
        dgl_rect(&s, 40, 32, 560, 296, DGL_RGB(232, 232, 216));
    }
    printf("hash %016llx\n", (unsigned long long)dgl_hash(&s));
    if (dgl_save_ppm(&s, out) != DGL_OK) {
        dgl_surface_free(&s);
        return 1;
    }
    dgl_surface_free(&s);
    return 0;
}

static void usage(void) {
    puts("DEADGL " DGL_VERSION);
    puts("usage:");
    puts("  deadgl --version");
    puts("  deadgl demo shrine -o out.ppm");
    puts("  deadgl demo depth -o out.ppm");
    puts("  deadgl run scene.dgl -o out.ppm");
    puts("  deadgl hash scene.dgl");
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        puts("DEADGL " DGL_VERSION);
        return 0;
    }
    if (argc >= 5 && strcmp(argv[1], "demo") == 0 && strcmp(argv[3], "-o") == 0) {
        return demo(argv[2], argv[4]);
    }
    if (argc >= 5 && strcmp(argv[1], "run") == 0 && strcmp(argv[3], "-o") == 0) {
        return run_scene(argv[2], argv[4], 0);
    }
    if (argc == 3 && strcmp(argv[1], "hash") == 0) {
        return run_scene(argv[2], "", 1);
    }
    usage();
    return argc == 1 ? 0 : 1;
}
