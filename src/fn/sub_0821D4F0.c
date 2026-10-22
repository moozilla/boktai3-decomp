#include "terrain_cell_probe.h"

#ifdef NONMATCHING
// fuzzy: 90.5%
// ROM keeps the dead `r = 0` (r3) so r lands in r3 with caller-save around the call;
// here r is allocated a callee-saved register and i/&next swap r5/r7.
s32 sub_0821D178(struct Pos16 *out, struct Pos16 *from, s32 arg);

static inline u32 cellHeightAt(struct Pos16 *pos, s32 cell)
{
    u8 *props = cellProps(cell);
    u32 type = *props >> 4;
    u32 h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)pos->z;
            break;
        case 2:
            h -= (u8)pos->x;
            break;
    }
    return h;
}

// Walks up to `steps` path steps from `start`; returns 1 when the walk finishes
// (sub_0821D178 returns 1) before the terrain rises `maxRise` above start->y, else 0.
s32 sub_0821D4F0(struct Pos16 *start, s32 arg, s32 steps, s32 maxRise)
{
    s32 k2 = 0;
    struct Pos16 cur, next;
    s32 i, cell, r;

    r = 0;
    if (gUnk_030052F4->cells == NULL)
        return 0;
    cur = *start;
    cell = cellIndexAt(&cur);
    for (i = k2; i < steps; i++) {
        r = sub_0821D178(&next, &cur, arg);
        do {
        } while (0);
        if (r > 1)
            cell = cellIndexAt(&next);
        if ((s32)(cellHeightAt(&next, cell) - (s16)start->y) >= maxRise)
            return 0;
        if (r == 1)
            return 1;
        cur = next;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821D4F0);
#endif
