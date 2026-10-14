#ifdef NONMATCHING
// fuzzy: 65.6%
/* Distance/status gate and orthogonal slope routes are understood; register lifetimes and branch layout still differ. */
#include "tether_actor.h"
extern u8 *gUnk_02000488;
u32 TetherActor_TestMatchingStatus(u32, u32, u32, u32);
u8 *sub_0821D64C(s32, s32);
static inline s32 cellIndex(s32 x, s32 z)
{
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    return index;
}
static inline s32 terrainHeight(struct ContactPackedVector *position, s32 index)
{
    u8 *properties = sub_0821D64C(index, 1);
    u32 surface, height;
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = index * 4 + 12;
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
    return height;
}
static inline s32 absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
u32 TetherActor_TestNearbyInteraction(struct ContactPosition *position, u32 tag, u32 flags)
{
    struct TetherActor *actor = (struct TetherActor *)gUnk_02000488;
    s32 dx, dz, ownIndex, otherIndex;
    u32 state;
    u16 rawX, rawZ;
    struct ContactPackedVector corner;
    u32 result;
    if (!actor)
        goto no;
    dx = position->x - actor->position.x;
    dz = position->z - actor->position.z;
    if ((s32)((u32)dx * dx + (u32)dz * dz) > 48400)
        goto no;
    state = actor->state;
    if ((u8)(state - 16) <= 1)
        goto yes;
    if (state != 0 && state != 3 && state != 4)
        goto no;
    actor = (struct TetherActor *)gUnk_02000488;
    if (!TetherActor_TestMatchingStatus(tag, flags, actor->statusTag, actor->statusFlags))
        goto no;
    rawX = position->x;
    rawZ = position->z;
    ownIndex = cellIndex((s16)rawX >> 8, (s16)rawZ >> 8);
    actor = (struct TetherActor *)gUnk_02000488;
    otherIndex = cellIndex(actor->position.x >> 8, actor->position.z >> 8);
    if (ownIndex == otherIndex)
        goto yes;
    if (ownIndex == otherIndex + 1 || ownIndex == otherIndex - 1 ||
        ownIndex == otherIndex + ((s16 *)gUnk_030052F4->cells)[2] ||
        ownIndex == otherIndex - ((s16 *)gUnk_030052F4->cells)[2]) {
        actor = (struct TetherActor *)gUnk_02000488;
        if (absolute(position->y - actor->position.y) <= 255)
            goto yes;
        goto no;
    }
    actor = (struct TetherActor *)gUnk_02000488;
    if (absolute(position->y - actor->position.y) > 255)
        goto no;
    corner.x = rawX - dx;
    corner.z = rawZ;
    ownIndex = cellIndex(corner.x >> 8, corner.z >> 8);
    if (absolute(position->y - terrainHeight(&corner, ownIndex)) <= 127)
        goto yes;
    corner.x = position->x;
    corner.z = position->z - dz;
    ownIndex = cellIndex(corner.x >> 8, corner.z >> 8);
    if (absolute(position->y - terrainHeight(&corner, ownIndex)) <= 127)
        goto yes;
no:
    result = 0;
    goto done;
yes:
    result = 1;
done:
    return result;
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", TetherActor_TestNearbyInteraction);
#endif
