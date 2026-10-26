// MATCHING_CONCESSION: register-pinning
#include "breakable_prop.h"
static inline u32 cellAtPosition(struct ContactPosition *position)
{
    s16 *xAddress = (s16 *)position;
    s32 x, z;
    x = *xAddress >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 firstCellAtPosition(struct ContactPosition *position)
{
    register struct ContactPosition *source asm("r6") = position;
    s16 *xAddress = (s16 *)source;
    s32 x, z;
    asm("" : "+r"(xAddress));
    x = *xAddress >> 8;
    z = source->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 BreakableProp_InitializeFromScript(struct BreakableProp *input, u32 id, u32 unused)
{
    struct BreakableProp *prop = input;
    struct ContactPackedVector packet;
    register struct ContactPosition *position asm("r6");
    u32 cell, height, surface;
    u8 *properties;
    u32 script;
    asm("" : "+r"(prop));
    if (Script_SeekToKeyword('p')) {
        packet.x = ((Script_GetValue() >> 8) << 8) + 128;
        packet.y = Script_GetValue();
        packet.z = ((Script_GetValue() >> 8) << 8) + 128;
    } else {
        packet.x = 0;
        packet.y = 0;
        packet.z = 0;
    }
    position = (void *)&packet;
    cell = firstCellAtPosition(position);
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    {
        struct ContactPosition *point = (void *)&packet;
        u32 cellValue;
        point->y = height;
        cellValue = cellAtPosition(point);
        prop->cell = cellValue;
    }
    {
        struct ContactPosition *point = (void *)&packet;
        prop->height = (point->y >> 8) + 1;
    }
    TerrainModifiers_RegisterCell(prop->terrainRegistration, prop->cell, 0, prop->height, 255,
                                  0x103);
    BreakableProp_InitializeSprite(prop, &packet);
    BreakableProp_InitializeCollider(prop, id, &packet);
    script = Script_SeekToKeyword('R');
    if (script)
        prop->scriptId = Script_GetValue();
    else
        prop->scriptId = script;
    return 0;
}
