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
void sub_08031EAC(struct TextState *p)
{
    if (p->wrap == 1)
        goto clear;
    if (p->y - p->top < p->height) {
        p->x = p->left;
        p->y += 2;
    }
    return;
clear:
    p->wrap = 0;
}
