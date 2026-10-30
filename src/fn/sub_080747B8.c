#include "global.h"
#include "engine.h"
#include "scene_actor_reset.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
struct BurstExtent {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
void SceneParticleBurst_CreateWithPause(struct ContactPosition *, struct ContactPosition *,
                                        struct BurstExtent *, s32, s32, s32);
static inline s32 motionSquared(struct ContactPosition *motion)
{
    return motion->x * motion->x + motion->y * motion->y + motion->z * motion->z;
}
static inline s32 halve(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void CpuSetC(const void *src, void *dst, u32 control)
{
    CpuSet(src, dst, control);
}
static inline void orFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
s32 SceneActorMovement_UpdateAirborne(struct SceneActorRegistryResetView *actor)
{
    u8 category;
    u32 *flags;
    u32 value;
    u32 index;
    struct ContactPosition *position;
    const struct ContactPosition *local;
    u8 *cell;
    s32 height, kind;
    vu32 zero;
    struct BurstExtent extent;
    struct BurstExtent *burst;
    if (motionSquared(&actor->secondaryMotion) == 0 && *(s16 *)&actor->field2EA == 0) {
        u32 *clear = &actor->flags2D0;
        u32 mask = ~0x10;
        *clear &= mask;
        return 1;
    }
    category = actor->category;
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x100000;
        value = actor->flags2D0 & mask;
        flags = &actor->flags2D0;
    }
    if (!value) {
        u16 *y = (u16 *)&actor->secondaryMotion.y;
        s32 dy;
        *y = dy = *y - 2;
        actor->field2EA += dy;
        actor->body.motion.x += actor->secondaryMotion.x;
        actor->body.motion.z += actor->secondaryMotion.z;
        index = actor->terrainCell;
        local = &actor->body.position;
        cell = TerrainModifiers_FindCellOverride(index, 1);
        position = (void *)local;
        if (cell) {
            cell += 4;
            goto haveCell;
        }
    } else {
        if (Mod(actor->field2CA, 4) == 0) {
            actor->secondaryMotion.x = halve(actor->secondaryMotion.x);
            actor->secondaryMotion.z = halve(actor->secondaryMotion.z);
        }
        actor->body.motion.x += actor->secondaryMotion.x;
        actor->body.motion.z += actor->secondaryMotion.z;
        return 0;
    }
    cell = gTerrainMap->cells + (index * 4 + 12);
haveCell:
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)local + 4);
    goto done;
slopeX:
    height -= *(u8 *)local;
done:;
    {
        s16 *elevation = (s16 *)&actor->field2EA;
        if (*elevation + height <= height) {
            u16 *bouncePtr;
            u16 bounce;
            extent.x = 0x52;
            extent.y = 0;
            extent.z = 0x52;
            bouncePtr = (u16 *)((u8 *)actor + 0x2e8);
            bounce = *bouncePtr;
            burst = &extent;
            if (bounce == 0) {
                struct ContactPosition *dst = &actor->secondaryMotion;
                zero = bounce;
                CpuSetC((void *)&zero, dst, 0x05000002);
                *elevation = bounce;
            } else {
                s32 y;
                actor->secondaryMotion.x = Div(actor->secondaryMotion.x * 5, 10);
                y = actor->secondaryMotion.y;
                if (y < 0)
                    y = -y;
                actor->secondaryMotion.y = Div(*bouncePtr * y, 10);
                actor->secondaryMotion.z = Div(actor->secondaryMotion.z * 5, 10);
            }
            if (category != 0x11 && category != 0x17 && category != 0x13 && category != 0x12 &&
                category != 0xe && category != 0xc)
                SceneParticleBurst_CreateWithPause(position, &actor->secondaryMotion, burst, 6, 2,
                                                   4);
            if (*(void **)((u8 *)actor + 0x240))
                Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x240));
            actor->field2EA = 0;
        }
    }
    if (motionSquared(&actor->secondaryMotion) == 0 && *(s16 *)&actor->field2EA == 0) {
        actor->field2EA = 0;
        zero = 0;
        CpuSetC((void *)&zero, &actor->secondaryMotion, 0x05000002);
        orFlags(flags, 0x10);
        return 1;
    }
    return 0;
}
