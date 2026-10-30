#ifndef GUARD_SCENE_ACTOR_REACTION_H
#define GUARD_SCENE_ACTOR_REACTION_H
#include "scene_actor_reset.h"
#include "scene_actor_restoration.h"
#include "scene_actor_animation.h"
#include "scene_actor_placement.h"
/* Tentative reaction callbacks. This overlays only fields directly used by the
 * 080800A4 cohort; the shared reset-phase view owns the common prefix. */
struct SceneActorReactionGroup {
    u8 unknown0[0x24];
    void *collider;
    u8 unknown28[0x18];
    u16 flags414, field416, timer418;
    u8 unknown46[6];
    struct SceneActorRegistryControl control;
};
struct SceneActorReactionCommand {
    u8 unknown0[8];
    u8 first, second, kind, unknownB;
    u32 duration;
    void *payload;
};
struct SceneActorReactionTail {
    u8 unknown0[0x5D4];
    struct ContactPosition *destination;
};
void SceneActorControl_OnIdle(void);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
static inline u8 SceneActorReaction_TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline void SceneActorReaction_ClearRoute(struct SceneActorRegistryResetView *actor)
{
    actor->flags2CC &= ~0x141C0;
    actor->group.common.control.counter = 0;
    *(u16 *)((u8 *)&actor->group.common.control + 0x14) = 0;
    actor->group.common.control.timer = 0;
}
static inline void SceneActorReaction_Show(struct SceneActorRegistryResetView *actor)
{
    if (actor->drawableKind == 0)
        *(u32 *)actor->drawable &= ~1;
    else
        *(u32 *)((u8 *)actor->drawable + 0x28) &= ~1;
}
static inline void SceneActorReaction_ResetAnimation(struct SceneActorRegistryResetView *actor,
                                                     u32 phase)
{
    actor->state2A8 = 3;
    actor->phase2A9 = phase;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SceneActorReaction_SetControl(struct SceneActorRegistryResetView *actor,
                                                 u32 kind, void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    actor->flags114 &= ~1;
    actor->word104 = 0;
}
static inline u32 SceneActorReaction_TerrainHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell, flags, shape, height;
    u8 *data;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + cell * 4 + 12;
    flags = *data;
    shape = flags >> 4;
    height = (flags & 15) << 8;
    if (shape == 1)
        height -= *(u8 *)&position->z;
    else if (shape == 2)
        height -= *(u8 *)&position->x;
    return height;
}
#endif
