#ifndef GUARD_SCENE_ACTOR_RESET_H
#define GUARD_SCENE_ACTOR_RESET_H
#include "scene_actor_registry.h"
#include "scene_actor_state.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Route_Start(struct SceneActorRegistryRoute *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
void ProximityVolume_TranslateBounds(void *, struct ContactPosition *);
#endif
