#ifndef GUARD_SHADOW_REGISTRY_H
#define GUARD_SHADOW_REGISTRY_H
#include "terrain_shadow.h"

struct ShadowRegistryEntry {
    u16 id;
    u8 mode, unknown3;
    struct SceneVec position;
    struct TerrainShadow shadow;
    struct ShadowRegistryEntry *previous, *next;
};
struct ShadowRegistry {
    struct Task task;
    struct ShadowRegistryEntry *head;
};
/* The script reader packs each operand into one halfword. */
struct ShadowScriptPosition {
    s32 x : 16, y : 16, z : 16, unused : 16;
};
struct ShadowRegistry *ShadowRegistry_GetTask(void);
struct ShadowRegistryEntry *ShadowRegistry_FindEntry(struct ShadowRegistry *, u32);
s32 ShadowRegistry_LinkEntry(struct ShadowRegistry *, struct ShadowRegistryEntry *);
s32 ShadowRegistry_UnlinkEntry(struct ShadowRegistry *, struct ShadowRegistryEntry *);
u32 ShadowRegistry_OnUpdate(void);
u32 ShadowRegistry_OnDestroy(struct ShadowRegistry *);
s32 ShadowRegistry_InitTask(struct ShadowRegistry *);
struct ShadowRegistry *ShadowRegistry_CreateTask(void);
s32 ShadowRegistry_Create(s32, s32, struct SceneVec *);
s32 ShadowRegistry_CreateFromScript(void);
s32 ShadowRegistry_Remove(s32);
s32 ShadowRegistry_RemoveFromScript(void);
s32 ShadowRegistry_SetEntryMode(u32, u32);
s32 ShadowRegistry_SetModeFromScript(void);
s32 ShadowRegistry_SetEntryPosition(u32, struct SceneVec *);
s32 ShadowRegistry_SetPositionFromScript(void);
void ParticleManager_ClearTaskPtr(void);
#endif
