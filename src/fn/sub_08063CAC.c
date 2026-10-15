#include "roaming_object.h"
u8 *sub_0821D64C(s32, s32);
void sub_08225AB4(void *, u16, struct ContactPackedVector *, u32, u32, struct RoamingObject *);
void RoamingObject_InitializePosition(struct RoamingObject *owner)
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
    if (x < 0 || z < 0 || x >= gUnk_030052F8 || z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    definition = sub_0821D64C(cell, 1);
    if (definition)
        definition += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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
    sub_08225AB4((u8 *)owner + 0x1C, owner->id, &position, 0, 7, owner);
}
