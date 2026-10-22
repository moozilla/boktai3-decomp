#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 86.1%
// Structure matches; left: 'to' and the grid pointer swap r5/r6, one add operand order.
struct Pos {
    s16 x, y, z, w;
} __attribute__((aligned(4)));
struct SurfaceTrack {
    u8 pad0[4];
    u16 prevCell;
    u16 cell;
};
s32 Mod(s32, s32);
s32 Div(s32, s32);
u32 sub_0821BF5C(struct Pos *from, struct Pos *to, u8 kind);

static inline s32 cellIndex(struct Pos *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

#define WIDTH (*(s16 *)(grid + 4))
#define LO(v) (((v) << 8) + 1)
#define HI(v) ((((v) + 1) << 8) - 1)

u32 sub_0821C254(struct SurfaceTrack *t, struct Pos *from, struct Pos *to, u8 kind)
{
    struct Pos p;
    u8 *grid;
    u16 idx;
    s32 diff;

    if (gUnk_030052F4->cells == 0)
        return 0;
    grid = gUnk_030052F4->cells;
    idx = cellIndex(to);
    if (t->cell == idx || sub_0821BF5C(from, to, kind))
        return 0;
    diff = idx - t->cell;
    if (diff <= -WIDTH + 1)
        diff = (diff + 1) + WIDTH;
    else if (diff == -1)
        diff = 3;
    else if (diff == 1)
        diff = 4;
    else if (diff >= WIDTH - 1)
        diff = diff + (6 - WIDTH);
    switch ((u32)diff) {
        case 0:
            if (t->prevCell == idx + 1) {
                p = *to;
                p.x = LO(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = LO(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = LO(Mod(t->cell, WIDTH));
                to->z = LO(Div(t->cell, WIDTH));
                break;
            } else {
                p = *to;
                p.z = LO(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = LO(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = LO(Mod(t->cell, WIDTH));
                to->z = LO(Div(t->cell, WIDTH));
                break;
            }
            break;
        case 1:
            to->z = LO(Div(t->cell, WIDTH));
            break;
        case 2:
            if (t->prevCell == idx - 1) {
                p = *to;
                p.x = HI(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = LO(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = HI(Mod(t->cell, WIDTH));
                to->z = LO(Div(t->cell, WIDTH));
                break;
            } else {
                p = *to;
                p.z = LO(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = HI(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = HI(Mod(t->cell, WIDTH));
                to->z = LO(Div(t->cell, WIDTH));
                break;
            }
            break;
        case 3:
            to->x = LO(Mod(t->cell, WIDTH));
            break;
        case 4:
            to->x = HI(Mod(t->cell, WIDTH));
            break;
        case 5:
            if (t->prevCell == idx + 1) {
                p = *to;
                p.x = LO(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = HI(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = LO(Mod(t->cell, WIDTH));
                to->z = HI(Div(t->cell, WIDTH));
                break;
            } else {
                p = *to;
                p.z = HI(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = LO(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = LO(Mod(t->cell, WIDTH));
                to->z = HI(Div(t->cell, WIDTH));
                break;
            }
            break;
        case 6:
            to->z = HI(Div(t->cell, WIDTH));
            break;
        case 7:
            if (t->prevCell == idx - 1) {
                p = *to;
                p.x = HI(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = HI(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
            } else {
                p = *to;
                p.z = HI(Div(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = HI(Mod(t->cell, WIDTH));
                if (sub_0821BF5C(from, &p, kind)) {
                copy:
                    *to = p;
                    break;
                }
            }
            to->x = HI(Mod(t->cell, WIDTH));
            to->z = HI(Div(t->cell, WIDTH));
            break;
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821C254);
#endif
