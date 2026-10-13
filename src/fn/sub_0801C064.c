#include "global.h"
typedef struct {
    u32 pad[2], flags;
    u8 tail[84];
} Graphic;
typedef struct {
    u16 state, time;
} State;
void sub_0821980C(void *, void *, int, int);
void ResultMenu_EnqueueMarker(u8 *p)
{
    State *s = (State *)(p + 0x78c + p[0x4a] * 4);
    Graphic *g = (Graphic *)(p + 0x384 + p[0x4a] * 0x60);
    sub_0821980C(g, p + 0x104, p[0xbf] + 0xc2, 0);
    g->flags &= ~0x201;
    s->state = 1;
    s->time = 0;
    if (++p[0x4a] > 4)
        p[0x4a] = 0;
}
