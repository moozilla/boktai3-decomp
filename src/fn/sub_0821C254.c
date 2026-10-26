// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

struct Pos {
    s16 x, y, z, w;
} __attribute__((aligned(4)));
struct TerrainGrid {
    u32 unused;
    s16 width;
};
struct SurfaceTrack {
    u8 pad0[4];
    u16 prevCell;
    u16 cell;
};
s32 Mod(s32, s32);
s32 Div(s32, s32);
u32 sub_0821BF5C(struct Pos *from, struct Pos *to, u8 kind);

#define WIDTH (grid->width)
#define LO(v) (((v) << 8) + 1)
#define HI(v) ((((v) + 1) << 8) - 1)

u32 sub_0821C254(struct SurfaceTrack *t, struct Pos *from, struct Pos *destination, u32 kindArg)
{
    register struct Pos *to asm("r5") = destination;
    u8 kind = kindArg;
    struct Pos p;
    struct TerrainGrid *grid;
    u16 idx;
    s32 diff;
    if (gTerrainMap->cells == 0)
        return 0;
    grid = (struct TerrainGrid *)gTerrainMap->cells;
    {
        s32 x = to->x >> 8, z = to->z >> 8;
        s32 index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        idx = index;
    }
    if ((t->cell == idx) || sub_0821BF5C(from, to, kind))
        return 0;
    diff = idx - t->cell;
    if (diff <= ((-(grid->width)) + 1)) {
        s32 delta = diff + 1;
        diff = delta + grid->width;
    } else if (diff == (-1))
        diff = 3;
    else if (diff == 1)
        diff = 4;
    else if (diff >= ((grid->width) - 1))
        diff = diff + (6 - (grid->width));
    switch ((u32)diff) {
        case 0:
            if (t->prevCell == (idx + 1)) {
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            } else {
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            }
            break;

        case 1:
            to->z = (Div(t->cell, grid->width) << 8) + 1;
            break;

        case 2:
            if (t->prevCell == (idx - 1)) {
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            } else {
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            }
            break;

        case 3:
            to->x = (Mod(t->cell, grid->width) << 8) + 1;
            break;

        case 4:
            to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
            break;

        case 5:
            if (t->prevCell == (idx + 1)) {
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                break;
            } else {
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                break;
            }
            break;

        case 6:
            to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
            break;

        case 7:
            if (t->prevCell == (idx - 1)) {
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
            } else {
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (sub_0821BF5C(from, &p, kind)) {
                copy:
                    *to = p;

                    break;
                }
            }
            to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
            to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
            break;
    }

    return 1;
}
