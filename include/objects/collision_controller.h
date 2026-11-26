#ifndef COLLISION_CONTROLLER_H
#define COLLISION_CONTROLLER_H
#include "global.h"
#include "objects/scene_object.h"
#include "player/player.h"
#include "engine/ram.h"
#include "engine/task.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite_drawable_api.h"

// Tentative: a pool of 16 small hit-detecting props (idle sprite, hit animation, then spawns an
// effect burst) owned by a task, created from script. Each slot embeds a collider
// (struct SceneCollider), a sprite record and an animation player.

// Update modes: 0 idle, 1 hit delay, 2 hit animation.
struct CollisionSlot {
    u16 id;
    u8 mode;
    u8 type;
    u8 kind;
    u8 active;
    u8 finished; // set when the slot is done and should be released
    u8 entered;  // set to 1 when a new mode starts
    u8 done;     // animation finished this frame
    u8 pad9;
    u16 timer, delay;
    u16 attack, range, script; // effect parameters from the script
    struct SceneCollider collider;
    struct SpriteDrawable sprite;
    struct DisplaySequenceState anim;
};

struct CollisionManager {
    struct Task task;
    u8 paused;
    u8 pad19[3];
    struct SpriteEffectTileResource *graphics;          // hit sprite graphics
    struct SpriteEffectTileResource *animationGraphics; // idle sprite graphics
    void *hitResource;
    void *idleResource;
    struct CollisionSlot slots[16];
};

/* The other party of a collision callback (guess: its actor); only the hit flag is used here. */
struct CollisionOther {
    u8 pad[0x43];
    u8 hit;
};

/* 0x0200003C */
extern struct CollisionManager *gCollisionManager;
extern void *gUnk_08604A00[2][3];

void StrikeTrap_ClearGlobal(void);
struct CollisionSlot *StrikeTrap_FindFreeSlot(struct CollisionManager *);
s32 StrikeTrap_InitSlot(struct CollisionManager *, struct CollisionSlot *);
u32 StrikeTrap_RemoveSlot(struct CollisionManager *, struct CollisionSlot *);
void StrikeTrap_ReceiveHit(struct SceneCollider *, struct CollisionOther *,
                                    struct CollisionSlot *);
void StrikeTrap_UpdateIdleAnimationPaused(u32, struct CollisionSlot *);
void StrikeTrap_UpdateIdleAnimation(struct CollisionManager *, struct CollisionSlot *);
void StrikeTrap_UpdateHitDelayAnimation(u32, struct CollisionSlot *);
void StrikeTrap_UpdateHitAnimation(struct CollisionManager *, struct CollisionSlot *);
s32 StrikeTrap_UpdateAll(struct CollisionManager *);
s32 StrikeTrap_DestroyManager(struct CollisionManager *);
s32 StrikeTrap_InitManager(struct CollisionManager *, void *owner);
struct CollisionManager *StrikeTrap_CreateManager(void *owner, u32);
s32 StrikeTrap_ScriptCreate(void);
s32 StrikeTrap_ScriptGetActiveCount(void);
void StrikeTrap_ScriptRemoveAll(void);

/* Engine helpers. */
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, s32, s32);
s32 EffectManager_SpawnParticles(s32, u32, u32, void *, void *, s32, s32);

#endif
