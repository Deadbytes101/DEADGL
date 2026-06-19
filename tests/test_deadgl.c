#include "deadgl.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

int main(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    uint32_t c = 0;
    CHECK(strcmp(dgl_version(), "2.4.0") == 0);
    CHECK(dgl_surface_init(&s, 64, 48) == DGL_OK);
    CHECK(dgl_surface_valid(&s));
    CHECK(dgl_surface_pixel_count(&s) == 64u * 48u);
    CHECK(dgl_parse_color("#112233", &c) == DGL_OK);
    CHECK(c == DGL_RGB(0x11, 0x22, 0x33));
    dgl_clear(&s, DGL_RGB(1, 2, 3));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(1, 2, 3));
    dgl_line(&s, 0, 0, 63, 47, DGL_RGB(7, 7, 7));
    CHECK(dgl_save_ppm(&s, "deadgl_test.ppm") == DGL_OK);
    remove("deadgl_test.ppm");
    dgl_surface_free(&s);
    puts("PASS deadgl core");
    return 0;
}
