#ifndef GUARD_SCENE_SCRIPT_OBJECT_H
#define GUARD_SCENE_SCRIPT_OBJECT_H
#include "engine/engine.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "scene/collision/contact_collider.h"
#include "engine/video/display_sequence.h"
struct SceneScriptResource {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    const void *paletteData;
    u8 unknown10[0xC];
    struct DisplaySequenceState frames;
    u16 palette[16];
};
struct SceneScriptObject {
    struct Task task;
    struct ContactSwarmSprite sprite;
    struct SceneScriptResource resource;
    void *proc;
    struct ContactCollider collider;
    u32 caller;
    s16 state, lifetime, reveal, fadeTimer, holdTimer, parameter;
    u32 pendingScript;
    u8 shadow[0x40];
    u16 finished, counter;
};
struct SceneScriptCallbacks {
    void *entries[2];
};
void DungeonScenery_RunPendingScript(struct SceneScriptObject *);
void DungeonScenery_UpdateFade(struct SceneScriptObject *);
void DungeonScenery_UpdateBurst(struct SceneScriptObject *);
void DungeonScenery_Update(struct SceneScriptObject *);
void DungeonProp_Destroy(struct SceneScriptObject *);
void DungeonScenery_ApplyHitFlash(struct ContactCollider *, void *, struct SceneScriptObject *);
void DungeonScenery_InitHitCollider(struct SceneScriptObject *);
s32 DungeonScenery_Initialize(struct SceneScriptObject *, u32);
#endif
