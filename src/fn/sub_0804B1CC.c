// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_TestTerrainTransition sub_0804B1CC
u8 *sub_0821D64C(s32, s32);
static inline u8 *propertiesAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 cell;
    u8 *properties;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}
u32 TetherActor_TestTerrainTransition(struct TetherActor *actor)
{
    struct ContactPosition *position;
    u8 *properties;
    s32 height, difference;
    u32 surface, result;
    if (!actor->terrainCheckReady) {
        ++actor->terrainCheckReady;
        goto no;
    }
    if (actor->state == 5)
        goto no;
    if (actor->statusFlags)
        goto no;
    {
        register struct ContactPosition *first asm("r0") = &actor->position;
        s32 x = first->x >> 8;
        s32 z = first->z >> 8;
        s32 cell;
        asm("" : : "r"(first), "r"(x), "r"(z));
        position = first;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            cell = 0;
        else
            cell = gUnk_030052F4->rows[z] + x;
        properties = sub_0821D64C(cell, 1);
        if (properties)
            properties += 4;
        else {
            struct EngineMap *map = gUnk_030052F4;
            u32 offset = cell * 4 + 12;
            properties = map->cells + offset;
        }
    }
    {
        u32 attributes = *(u16 *)(properties + 2);
        attributes &= 64;
        if (attributes)
            goto yes;
    }
    if (actor->state == 1)
        goto no;
    properties = propertiesAt(position);
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
    difference = height - actor->position.y;
    if (difference < 0)
        difference = -difference;
    if (difference <= 255)
        goto no;
yes:
    result = 1;
    goto done;
no:
    result = 0;
done:
    return result;
}
