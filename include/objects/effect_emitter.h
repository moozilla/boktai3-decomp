#ifndef EFFECT_EMITTER_H
#define EFFECT_EMITTER_H
#include "global.h"
#include "scene/collision/contact_collider.h"
#include "objects/scene_object.h"
#include "engine/task.h"

// Tentative: a hovering, bobbing prop with a shadow that can be hit; when its health runs out it
// bursts into an effect (EffectManager_SpawnParticlesAt), see homing_effect.h.

struct EmitterVec {
    s16 x, y, z, pad;
};

struct EmitterSprite {
    u32 flags; // bit 0: hidden
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct EmitterVec position;
    u8 tail[8];
};

struct EffectEmitter {
    struct Task task;
    u16 id, pad1A;
    u8 pad1C[4];
    void *scene; // the scene (owner) this emitter belongs to; hidden when not current
    s16 health;
    u16 maximum, cooldown, phase, timer, flash;
    u8 rising, pad31;
    s16 baseY, offset; // vertical bobbing around baseY
    u16 count, kind;   // effect parameters
    u16 pad3A;
    struct SceneAnimation resource;
    struct EmitterSprite sprite;
    u8 shadow[64];
    struct SceneCollider collider;
};

extern u16 gUnk_08604AB4[];

void EffectEmitter_ReceiveHit(struct ContactCollider *, struct ContactCollider *, struct EffectEmitter *);
s32 EffectEmitter_Update(struct EffectEmitter *);
u32 EffectEmitter_Destroy(struct EffectEmitter *);
s32 EffectEmitter_Initialize(struct EffectEmitter *, u32, u32);
void *EffectEmitter_Create(u32, u32);

/* Engine helpers. */
s32 EffectManager_SpawnParticlesAt(s32, u32, s32, struct EmitterVec *, void *, s32, s32);
void *Region_FindContaining(void *);
void Shadow_Destroy(void *);
void Shadow_Initialize(void *, void *, u32);

#endif
