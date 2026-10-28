#ifndef COLLISION_CONTROLLER_H
#define COLLISION_CONTROLLER_H
#include "global.h"
#include "scene_object.h"
#include "player.h"
#include "ram.h"

// Tentative: a pool of 16 small hit-detecting props (idle sprite, hit animation, then spawns an
// effect burst) owned by a task, created from script. Each slot embeds a collider
// (struct SceneCollider), a sprite record and an animation player.

struct CollisionVec {
    s16 x, y, z, pad;
};

struct CollisionSprite {
    u32 flags; // bit 0: animation finished, bit 2/3: flip
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 pad0C[12];
    struct CollisionVec position;
    u8 tail[8];
};

struct CollisionAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

// Update modes (CollisionSlot.mode): 0 idle animation, 1 hit animation.
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
    struct CollisionSprite sprite;
    struct CollisionAnim anim;
};

struct CollisionGraphics {
    u32 pad;
    u16 base;
};

struct CollisionManager {
    u8 task[24];
    u8 paused;
    u8 pad19[3];
    struct CollisionGraphics *graphics;          // hit sprite graphics
    struct CollisionGraphics *animationGraphics; // idle sprite graphics
    void *hitResource;
    void *idleResource;
    struct CollisionSlot slots[16];
};

/* The other party of a collision callback (guess: its actor); only the hit flag is used here. */
struct CollisionOther {
    u8 pad[0x43];
    u8 hit;
};

struct CollisionScene {
    u8 pad[14];
    u8 mode;
};

extern struct CollisionManager *gCollisionManager;
extern struct CollisionScene gUnk_03005430;
extern u16 gUnk_02000534, gUnk_02000500;
extern void *gUnk_08604A00[2][3];

void sub_08009E30(void);
struct CollisionSlot *CollisionController_FindFreeSlot(struct CollisionManager *);
s32 CollisionController_InitSlot(struct CollisionManager *, struct CollisionSlot *);
u32 CollisionController_RemoveSlot(struct CollisionManager *, struct CollisionSlot *);
void CollisionController_ReceiveHit(struct SceneCollider *, struct CollisionOther *,
                                    struct CollisionSlot *);
void CollisionController_UpdateIdleAnimationPaused(u32, struct CollisionSlot *);
void CollisionController_UpdateIdleAnimation(struct CollisionManager *, struct CollisionSlot *);
void CollisionController_UpdateHitDelayAnimation(u32, struct CollisionSlot *);
void CollisionController_UpdateHitAnimation(struct CollisionManager *, struct CollisionSlot *);
s32 CollisionController_UpdateAll(struct CollisionManager *);
s32 CollisionController_DestroyManager(struct CollisionManager *);
s32 CollisionController_InitManager(struct CollisionManager *, void *owner);
struct CollisionManager *CollisionController_CreateManager(void *owner, u32);
s32 CollisionController_ScriptCreate(void);
s32 CollisionController_ScriptGetActiveCount(void);
void CollisionController_ScriptRemoveAll(void);

/* Engine helpers. */
void ParticleSprite_Initialize(void *, void *, u32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, s32, s32);
void EffectManager_SpawnParticles(u32, u32, u32, struct CollisionVec *, struct SceneExtent *, u32,
                                  u32);

#endif
