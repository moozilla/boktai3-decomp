#include "terrain_modifier.h"
static inline s32 heightLevel(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
static inline s32 cellIndex(struct TerrainVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
s32 sub_0821ABA8(s32, s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 TerrainModifiers_ChangeFromScript(void)
{
    s32 id = sub_0821ABA8('n', -1), tile = sub_0821ABA8('t', -1), kind = sub_0821ABA8('f', -1),
        flags = sub_0821ABA8('e', -1);
    struct TerrainScriptVec position;
    s32 cell;
    s32 height;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        cell = cellIndex((struct TerrainVec *)&position);
        height = ({
            s32 value = heightLevel(((struct TerrainVec *)&position)->y);
            value;
        });
    } else {
        height = -1;
        cell = height;
    }
    return TerrainModifiers_Change(id, tile, kind, flags, cell, height);
}
