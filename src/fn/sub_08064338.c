#include "roaming_object.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneBody_Init(void *, u16, struct ContactPackedVector *, u32, u32, struct RoamingObject *);
void RoamingObjectB_InitializePosition(struct RoamingObject *owner)
{
    struct ContactPackedVector position;
    u16 *point;
    s32 x, z;
    u32 cell;
    u8 *definition;
    u32 type;
    s32 height;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    point = (u16 *)&position;
    x = (s16)point[0] >> 8;
    z = (s16)point[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        definition = map->cells + offset;
    }
    type = *definition >> 4;
    height = (*definition & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point[2];
            break;
        case 2:
            height -= (u8)point[0];
            break;
    }
    ((u16 *)&position)[1] = height;
    SceneBody_Init((u8 *)owner + 0x1C, owner->id, &position, 0, 7, owner);
}
