#include "global.h"
typedef struct {
    signed x : 16;
    signed y : 16;
    signed z : 16;
    signed pad : 16;
} Vec;
typedef struct {
    u32 pad[2], flags;
    u8 gap[20];
    Vec position;
    u8 gap28[18];
    u16 palette;
    u8 gap3c[12];
    void *colors;
    u8 tail[20];
} Graphic;
void sub_08019E98(u8 *, Graphic *, int);
void *sub_08019E58(int);
void Anim_LoadKeyframe(void *, void *, int, int);
void SelectionMenu_PopulateActors(u8 *p)
{
    Graphic *g = (Graphic *)(p + 0x674);
    int i;
    Vec v;
    for (i = 0; i < 16; i++, g++) {
        v.x = (i & 3) * 24 + 144;
        v.y = (i >> 2) * 24 + 32;
        sub_08019E98(p, g, i);
        g->position = v;
        if ((1 << i) & *(u32 *)(p + 0x34)) {
            g->palette = 0x373;
            g->colors = sub_08019E58(0);
        } else {
            g->palette = 0x374;
            g->colors = sub_08019E58(1);
        }
    }
    g = (Graphic *)(p + 0x614);
    Anim_LoadKeyframe(g, p + 0x4c, 0x65, 0);
    g->flags &= ~1;
}
