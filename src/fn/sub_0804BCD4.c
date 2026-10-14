#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#include "game_state.h"
#define TetherActor_TestStepPosition sub_0804BCD4
u8 *sub_0821D64C(s32, s32);
u32 sub_08100AD4(struct ContactPosition *);
static inline u8 *propertiesAt(struct ContactPosition *input)
{
    register struct ContactPosition *position asm("r6") = input;
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
static inline u32 terrainFlagEnabled(void)
{
    u32 mask = 16;
    u32 flags;
    u32 result;
    flags = gUnk_02000710->flags868;
    if (flags & mask) {
        result = 1;
        asm("" : "+r"(result));
    } else {
        result = 0;
    }
    return result;
}
u32 TetherActor_TestStepPosition(struct ContactPosition *origin, struct ContactPosition *input, u32 inputMask)
{
    register struct ContactPosition *candidate asm("r6");
    u32 mask;
    u8 *properties;
    u32 attributes;
    s32 height, difference;
    u32 surface;
    candidate = input;
    asm("" : : "r"(candidate));
    mask = inputMask;
    properties = propertiesAt(candidate);
    attributes = *(u16 *)(properties + 2);
    mask &= attributes;
    if (mask)
        goto no;
    if ((attributes & 128) && terrainFlagEnabled())
        goto no;
    properties = propertiesAt(candidate);
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
    case 1: height -= (u8)candidate->z; break;
    case 2: height -= (u8)candidate->x; break;
    }
    difference = origin->y - height;
    if (difference < 0)
        difference = -difference;
    if (difference > 50)
        goto no;
    if (sub_08100AD4(candidate))
        goto no;
    return 1;
no:
    return 0;
}
