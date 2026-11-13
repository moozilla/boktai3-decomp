#ifndef GUARD_SPREAD_SHOT_H
#define GUARD_SPREAD_SHOT_H
#include "engine.h"
#include "scene_actor_components.h"
#include "contact_swarm_sprite.h"
#include "contact_collider.h"
struct SpreadShot {
    struct SceneActorBody body;
    struct ContactSwarmSprite sprite;
    struct ContactPosition position, velocity;
    struct ContactCollider collider;
    u16 hits;
    u8 kind, reset;
    u16 cell;
    s8 index;
    u8 state;
    u16 timer, unknownE2;
};
struct PairSlotPool {
    struct Task task;
    struct SpreadShot entries[2];
    u32 allocated;
};
struct SpreadShot *PairSlotPool_Allocate(struct PairSlotPool *);
struct PairSlotPool *SceneTerrainProbe_EnsurePool(void);
s32 SceneTerrainProbe_InitPool(struct PairSlotPool *);
s32 PairSlotPool_Update(struct PairSlotPool *);
void SceneTerrainProbe_OnPoolDestroy(void);
u8 SpreadShot_ResolveTerrain(struct SpreadShot *, s32, u32);
void SceneTerrainProbe_NoopCollisionHook(void);
void SceneTerrainProbe_NoopTrailHook(void);
#endif
