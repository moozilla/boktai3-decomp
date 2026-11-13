#ifndef GUARD_SCENE_SCRIPT_OBJECT_H
#define GUARD_SCENE_SCRIPT_OBJECT_H
#include "engine.h"
#include "contact_swarm_sprite.h"
#include "contact_collider.h"
#include "display_sequence.h"
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
void SceneScriptObject_RunPendingScript(struct SceneScriptObject *);
void SceneScriptObject_UpdateFade(struct SceneScriptObject *);
void SceneScriptObject_UpdateBurst(struct SceneScriptObject *);
void SceneScriptTask_Update(struct SceneScriptObject *);
void DungeonProp_Destroy(struct SceneScriptObject *);
void SceneScriptObject_ApplyHitFlash(struct ContactCollider *, void *, struct SceneScriptObject *);
void SceneScriptObject_InitHitCollider(struct SceneScriptObject *);
s32 SceneScriptObject_Initialize(struct SceneScriptObject *, u32);
#endif
