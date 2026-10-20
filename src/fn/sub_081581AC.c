#include "global.h"
#include "engine.h"
#include "player.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsStatusBit4ActionActive(u32);

s32 Player_TestMovementPosition(struct PlayerColliderVector *v)
{
    s32 x = *(s16 *)v >> 8;
    s32 z = *(s16 *)((u8 *)v + 4) >> 8;
    s32 cell;
    u8 *data;
    u32 flags;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    flags = *(u16 *)(data + 2);
    if (flags & 0x40)
        return 1;
    if (Player_IsStatusBit4ActionActive(flags))
        return 1;
    return 0;
}
