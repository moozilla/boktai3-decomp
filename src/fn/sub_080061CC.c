#include "global.h"
struct TerrainModifier {
    u16 id;
    u8 active, kind, tile, height;
    u16 flags, cell;
    u8 padA[2];
    u8 registration[16];
    struct TerrainModifier *previous, *next;
};
struct Manager {
    u8 pad[24];
    struct TerrainModifier *head;
};
struct Vec {
    s16 x, y, z, pad;
};
struct Map {
    u8 pad[36];
    u16 rows[1];
};
extern struct Manager *gUnk_02000028;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
static inline s32 heightLevel(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
static inline s32 cellIndex(struct Vec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
#define TerrainModifiers_ChangeFromScript sub_080061CC
struct ScriptVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
s32 sub_0821ABA8(s32, s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 sub_08006088(s32, s32, s32, s32, s32, s32);
s32 TerrainModifiers_ChangeFromScript(void)
{
    s32 id = sub_0821ABA8('n', -1), tile = sub_0821ABA8('t', -1), kind = sub_0821ABA8('f', -1),
        flags = sub_0821ABA8('e', -1);
    struct ScriptVec position;
    s32 cell;
    s32 height;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        cell = cellIndex((struct Vec *)&position);
        height = ({
            s32 value = heightLevel(((struct Vec *)&position)->y);
            value;
        });
    } else {
        height = -1;
        cell = height;
    }
    return sub_08006088(id, tile, kind, flags, cell, height);
}
