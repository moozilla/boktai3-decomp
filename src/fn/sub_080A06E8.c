#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

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
struct CrowTerrainState {
    u8 unknown0[0x3d4];
    u32 flags;
    u8 unknown3d8[0x54];
    s16 x, y, z;
};
static inline void Add(struct ContactPosition *p, struct ContactPosition *a)
{
    p->x = a->x + p->x;
    p->y = a->y + p->y;
    p->z = a->z + p->z;
}
static inline void Clear(struct CrowTerrainState *p, u32 m)
{
    p->flags &= ~m;
}
static inline void Set(struct CrowTerrainState *p, u32 m)
{
    p->flags |= m;
}
void Crow_StopAtRaisedTerrain(struct SceneActorRegistryResetView *actor)
{
    struct CrowTerrainState *context = actor->context;
    struct ContactPosition target;
    Clear(context, 2);
    if (actor->field2B5 == 0x25) {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->body.position;
        struct ContactPosition *offset = &actor->secondaryMotion;

        s32 cell, height;

        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        Add(p, &actor->body.motion);
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = Height(cell, &target);
        if (actor->groundHeight2BE + *(s16 *)((u8 *)actor + 0x2ea) < height) {
            actor->secondaryMotion.x = 0;
            actor->secondaryMotion.y = 0;
            actor->secondaryMotion.z = 0;
            context->x = 0;
            context->y = 0;
            context->z = 0;
            actor->body.motion.x = 0;
            actor->body.motion.z = 0;
            actor->body.motion.y = 0;
            Set(context, 2);
        }
        if (target.y < height) {
            actor->secondaryMotion.y = 0;
            actor->body.motion.y = 0;
            context->y = 0;
        }
    }
}
