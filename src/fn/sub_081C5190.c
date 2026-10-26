#include "global.h"
#include "engine.h"

struct S {
    u8 pad[0x38];
    s16 f38, f3a, f3c;
    u8 pad2[0xcc - 0x3e];
    s32 fcc;
    u8 pad3[0xd8 - 0xd0];
    u32 fd8;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Registry1909_StartLandingAnimation(struct S *p);

void Registry1909_UpdateFalling(struct S *p)
{
    u8 *q = (u8 *)p + 0x38;
    s32 x = p->f38 >> 8;
    s32 z = *(s16 *)(q + 4) >> 8;
    s32 cellIndex;
    u8 *cell;
    u32 kind;
    s32 height;

    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cellIndex = 0;
    else
        cellIndex = gTerrainMap->rows[z] + x;

    cell = TerrainModifiers_FindCellOverride(cellIndex, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + cellIndex * 4;

    kind = *cell >> 4;
    height = (*cell & 0xf) << 8;
    switch (kind) {
        case 1:
            height -= q[4];
            break;
        case 2:
            height -= q[0];
            break;
    }

    p->fcc -= 2;
    p->f3a += p->fcc;
    if (p->f3a < height) {
        p->f3a = height;
        if (p->fcc > -2) {
            Registry1909_StartLandingAnimation(p);
            return;
        } else {
            p->fcc = (-p->fcc * 3) / 4;
        }
    }
    p->fd8++;
}
