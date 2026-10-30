#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

static inline u32 TestFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    mask &= p->flags2CC;
    return mask;
}
static inline s32 Height(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= ((u8 *)p)[4];
            break;
        case 2:
            height -= ((u8 *)p)[0];
            break;
    }
    return height;
}
void RouteActor_ReflectAtRaisedTerrain(struct SceneActorRegistryResetView *actor)
{
    s16 *x_2;
    struct ContactPosition target;
    if (TestFlag(actor, 0x2000)) {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->body.position;
        struct ContactPosition *offset = &actor->secondaryMotion;
        s32 cell, height;
        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = Height(cell, &target);
        if (actor->groundHeight2BE < height) {
            s32 zero;
            s32 off = 0x2EC;
            s16 *x = (s16 *)((u8 *)actor + off);
            s32 value = -*x;
            zero = 0;
            *x = value;
            off = 0x2F0;
            x_2 = (s16 *)(((u8 *)actor) + off);
            value = -(*x_2);
            *x_2 = value;
            actor->body.motion.x = zero;
            actor->body.motion.z = zero;
        }
    }
}
