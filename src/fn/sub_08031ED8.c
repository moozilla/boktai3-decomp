#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u8 pad18[4];
    const u8 *pc;
    u8 pad20[0xE8];
    u8 depth, styles[7];
};
int sub_08031ED8(struct TextState *p, int wide)
{
    if (p->wrap != 2)
        p->wrap = 0;
    if (!wide)
        p->x++;
    else
        p->x += 2;
}
