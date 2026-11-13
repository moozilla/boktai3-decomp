#ifndef GUARD_PURSUING_ACTOR_H
#define GUARD_PURSUING_ACTOR_H
#include "engine.h"
#include "scene_vec.h"
#include "contact_collider.h"
#include "player_cue_sequence.h"
/* Tentative family: resource 8DC7, paired colliders and two short-lived trail sprites. */
struct PursuingActorSprite {
    u32 flags;
    u8 active, unknown5, palette, priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *resource;
    u16 frame;
    u8 unknown12[0xA];
    struct SceneVec position;
    u8 unknown24[8];
};
struct PursuingActorTrail {
    struct PursuingActorSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    u16 lifetime, unknown5A;
};
struct PursuingActor {
    u32 flags;
    u8 enabled, unknown5, phase, priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[0xA];
    struct SceneVec position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    struct PursuingActorTrail trail[2];
    struct ContactCollider primary, secondary;
    struct SceneVec velocity;
    u8 initialHeading, heading, turnTimer, turnSpeed;
    u16 speed;
    u8 trailIndex, trailPeriod;
    const void *sequenceResource;
    s32 lifetime;
    u16 effectFlags;
    u8 active;
    s8 slot;
    u16 timer;
    u8 entered, turnDelay;
};
struct PursuingActorManager {
    struct Task task;
    u8 graphics[0x20];
    u32 activeSlots;
    struct PursuingActor actors[8];
};
struct ActorColliderExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern void *gUnk_086107D8[];
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void TwinSpriteEffect_InitGraphics(struct PursuingActor *);
void TwinSpriteEffect_EmitTrail(struct PursuingActor *);
void TimedEffect_UpdateVisibility(struct PursuingActor *);
void SwooshProjectile_RemoveTrailSprites(struct PursuingActor *);
void SwooshProjectile_OnPrimaryColliderContact(u32, u32, struct PursuingActor *);
void SlashEffect_OnSecondaryColliderContact(u32, u32, struct PursuingActor *);
void PursuingActor_InitColliders(struct PursuingActor *, s32, s32, s32, s32, s32, s32, s32);
void SlashEffect_UpdateColliders(struct PursuingActor *);
struct PursuingActor *SlashEffect_AllocateSlot(struct PursuingActorManager *);
void PursuingActor_TurnTowardPlayer(struct PursuingActor *);
struct PursuingActorManager *SlashEffect_GetOrCreateManager(void);
s32 SlashEffect_InitManager(struct PursuingActorManager *);
s32 PursuingActor_UpdateManager(struct PursuingActorManager *);
u32 PursuingActors_ReleaseAll(struct PursuingActorManager *);
#endif
