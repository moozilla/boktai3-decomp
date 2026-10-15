// MATCHING_CONCESSION: register-pinning
#include "breakable_prop.h"
#define BreakableProp_InitializeFromScript sub_08051858
static inline u32 cellAtPosition(struct ContactPosition *position)
{
    s16 *xAddress = (s16 *)position;
    s32 x, z;
    asm("" : "+r"(xAddress));
    x = *xAddress >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 firstCellAtPosition(struct ContactPosition *position)
{
    register struct ContactPosition *source asm("r6") = position;
    s16 *xAddress = (s16 *)source;
    s32 x, z;
    asm("" : "+r"(xAddress));
    x = *xAddress >> 8;
    z = source->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
s32 BreakableProp_InitializeFromScript(struct BreakableProp *input, u32 id)
{
    struct BreakableProp *prop = input;
    struct ContactPackedVector packet;
    register struct ContactPosition *position asm("r6");
    u32 cell, height, surface;
    u8 *properties;
    register u32 script asm("r1");
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
    properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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
    sub_0821D698(prop->terrainRegistration, prop->cell, 0, prop->height, 255, 0x103);
    sub_0805175C(prop, &packet);
    sub_080517D0(prop, id, &packet);
    script = Script_SeekToKeyword('R');
    if (script)
        prop->scriptId = Script_GetValue();
    else
        prop->scriptId = script;
    return 0;
}
