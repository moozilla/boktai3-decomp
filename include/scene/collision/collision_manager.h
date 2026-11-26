#ifndef GUARD_REGISTRY_E7BC_H
#define GUARD_REGISTRY_E7BC_H

#include "scene/collision/spatial_index.h"

struct BoundsPosition {
    s32 x, y, z, unused;
};

struct RegistryE7BCObject {
    struct SpatialEntry collider; /* 0x00 */
    u32 procedure;                /* 0x40 */
    s32 active;                   /* 0x44 */
};

struct RegistryE7BCManager {
    struct Task task;
    struct RegistryE7BCObject objects[12];
};

union RegistryE7BCPosition {
    struct {
        u32 x : 16, y : 16, z : 16, unused : 16;
    } packed;
    struct ContactPosition coordinates;
};

s32 Bounds_TestOverlap(struct BoundsPosition *, struct ContactPosition *, struct BoundsPosition *,
                       struct ContactPosition *);
void Collision_DispatchOverlaps(struct SpatialEntry *, struct SpatialLink *);
s32 Collision_ProcessGroups(struct SpatialIndex *);
s32 CategoryLists_Destroy(struct SpatialIndex *);
void Collider_InitManagerBuckets(struct SpatialIndex *);
s32 Collider_InitManager(struct SpatialIndex *);
struct SpatialIndex *Collider_CreateManager(void);
void ScriptObjectPool_SetFlags(u32 *, u32 bits);
void ScriptObjectPool_ClearFlags(u32 *, u32 bits);
void ScriptObjectPool_OnObjectComplete(void *, struct SpatialEntry *);
s32 ScriptObjectPool_QueueActiveObjects(struct RegistryE7BCManager *);
void ScriptObjectPool_OnManagerDestroy(void);
s32 ScriptObjectPool_InitManager(struct RegistryE7BCManager *);
struct RegistryE7BCManager *ScriptObjectPool_CreateManager(void);
struct RegistryE7BCObject *ScriptObjectPool_FindFreeObject(struct RegistryE7BCManager *);
void ScriptObjectPool_CreateFromScript(void);
void ScriptObjectPool_ResetManager(void);
void GameState_SetResultIndex(s32);
void GameState_SetResultTimer(s32);
void GameState_SetActionCount892(s32);
void GameState_SetActionCount894(s32);
s16 GameState_GetResultIndex(void);
s32 GameState_GetResultTimer(void);
s16 GameState_GetActionCount892(void);
s16 GameState_GetActionCount894(void);
void GameState_AddActionCount892(s32);
void GameState_AddActionCount894(s32);
void GameState_ResetActionCounts(void);
s32 SceneTilemap_GetLayoutLength(void);

#endif
