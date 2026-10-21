// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "terrain_cell_probe.h"

struct SPos {
    s16 x, y, z, w;
};

struct PBar {
    u8 f00[0x70];
    u16 x70, y72, z74;
    u8 f76[0xB8 - 0x76];
    s32 st;
};

void AnimData_SelectPaletteBank(void *, s32);

s32 PlayerBarrier_TryPlace(void *task, struct SPos *sp)
{
    u8 *b = task;
    s16 v[4];
    s16 *q;
    int new_var;
    u32 mk;
    s32 x;
    s32 y;
    s32 m;
    s32 z;
    s32 cell = cellIndexAt((struct Pos16 *)sp);
    u32 attr = *(u16 *)(cellProps(cell) + 2);
    if (attr & 0x1083)
        return 0;
    if (TerrainModifiers_FindCellOverride(cellIndexAt((struct Pos16 *)sp), 0))
        return 0;
    x = sp->x;
    m = 0xFF00;
    x &= m;
    y = sp->y;
    z = sp->z & m;
    q = v;
    x += 0x16;
    z += 0x16;
    v[0] = x;
    q[1] = y;
    q[2] = z;
    {
        register u32 vx asm("r1");
        vx = *(u16 *)sp;
        mk = 0xFFFFFF00;
        new_var = (vx & mk) + 0x80;
    }
    ((struct PBar *)b)->x70 = new_var;
    ((struct PBar *)b)->y72 = sp->y;
    ((struct PBar *)b)->z74 = (((u16 *)sp)[2] & mk) + 0x80;
    ((struct PBar *)b)->st = 1;
    *(struct SPos *)(b + 0x24) = *(struct SPos *)v;
    *(struct SPos *)(b + 0x110) = *(struct SPos *)v;
    *(struct SPos *)(b + 0x13C) = *(struct SPos *)v;
    *(u16 *)(b + 0x13C) -= 0x40;
    *(u16 *)(b + 0x140) -= 0x40;
    *(u32 *)(b + 0xF4) &= ~1;
    AnimData_SelectPaletteBank(b + 0xBC, 0x65);
    *(u8 *)(b + 0xFB) = 2;
    return 1;
}
