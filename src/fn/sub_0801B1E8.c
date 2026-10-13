#include "global.h"
typedef struct {
    u32 pad[2], flags;
    u8 tail[84];
} Graphic;
extern u32 gUnk_0300523C;
void SelectionMenu_Hide(u8 *arg)
{
    u8 *p = arg;
    int i, mask;
    u32 *flags;
    u32 *state;
    Graphic *g;
    g = (Graphic *)(p + 0xd4);
    g->flags |= 1;
    g = (Graphic *)(p + 0x134);
    g->flags |= 1;
    state = &gUnk_0300523C;
    mask = 1;
    flags = (u32 *)(p + 0x67c);
    i = 18;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    mask = 1;
    flags = (u32 *)(p + 0x19c);
    i = 2;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    mask = 1;
    flags = (u32 *)(p + 0x2bc);
    i = 7;
    do {
        *flags |= mask;
        flags += 24;
        i--;
    } while (i >= 0);
    g = (Graphic *)(p + 0x5b4);
    g->flags |= 1;
    {
        int clear = ~4;
        *state &= clear;
    }
}
