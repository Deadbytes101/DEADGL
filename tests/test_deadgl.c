#include "deadgl.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

int main(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    CHECK(strcmp(dgl_version(), "11.0.0") == 0);
    CHECK(dgl_surface_init(&s, 8, 8) == DGL_OK);
    dgl_clear(&s, DGL_RGB(1, 2, 3));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(1, 2, 3));
    dgl_surface_free(&s);
    puts("PASS deadgl core");
    return 0;
}
