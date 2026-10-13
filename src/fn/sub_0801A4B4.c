#include "global.h"
typedef struct {
    u32 pad[2], flags;
    u8 gap[20];
    s16 x, y;
    u8 tail[60];
} Graphic;

void Anim_LoadKeyframe(void *, void *, int, int);
void SelectionMenu_PositionCursors(u8 *p)
{
    Graphic *g = (Graphic *)(p + 0x194);
    g->x = (p[0x1d] & 3) * 24 + 144;
    g->y = (p[0x1d] >> 2) * 24 + 32;
    g->flags &= ~1;
    Anim_LoadKeyframe(g, p + 0x4c, 0x4d, 0);
    g = (Graphic *)(p + 0x1f4);
    g->x = 16;
    g->y = 40;
    g->flags &= ~1;
    Anim_LoadKeyframe(g, p + 0x4c, 0x4e, 0);
    g = (Graphic *)(p + 0x254);
    g->x = 0;
    g->y = 0;
    g->flags |= 1;
    Anim_LoadKeyframe(g, p + 0x4c, 0x4b, 0);
}
