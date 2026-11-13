#ifndef GUARD_SCENE_ACTOR_UTILITIES_H
#define GUARD_SCENE_ACTOR_UTILITIES_H
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_route.h"
#include "scene_object.h"
#include "display_sequence.h"
#include "monster_particle_random.h"
struct SceneActorSequenceDrawable {
    struct SceneActorControlMotionDrawable drawable;
    void *sequenceResource;
};
struct SceneActorFacingBody {
    u8 unknown0[9], facing, unknownA[2];
    u16 x, unknownE, z;
};
struct SceneActorResponseState {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 threshold, control;
};
struct SceneActorRandomRoute {
    struct SceneActorRouteCursor cursor;
    u8 previousIndex;
};
s32 SceneActorTerrain_FindReachableCell(struct ContactPosition *, s32, s32);
s16 Math_GetVectorLength(s16, s16);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
u8 *Path_GetPointData(void *);
s32 Inventory_GetItemId(u32);
s32 Inventory_GetItemPackedState(u32);
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
#endif
