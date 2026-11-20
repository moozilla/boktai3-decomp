#include "global.h"
#include "player.h"
#include "axe.h"
#include "directional_sprite_state.h"
#include "engine.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain_reaction.h"

static inline struct DirectionalSprite *ActorPhase_GetDrawable(struct ChaseActor *actor)
{
    return actor->base.drawable;
}

static inline void Axe_SetEffectFlags(u32 bits, struct Axe *axe)
{
    axe->flagsE4C |= bits;
}

static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorPhase_EmitTrailSprite(struct ChaseActor *, const struct ContactPosition *, u32);

void Axe_UpdateTrailColliders(struct ChaseActor *);

void Axe_UpdateAttackColliderPositions(struct ChaseActor *);

void Axe_ReleaseAuxiliarySprite(struct ChaseActor *);

void SceneActorControl_ResetRotation(struct ChaseActor *);

void ActorPhase_UpdateAcceleratingTrail(struct ChaseActor *actor, s32 frame)
{
    struct Axe *axe = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        axe->auxiliarySprite.flags |= 2;
        axe->flagsCAB = 0;
        axe->trailHeading = 0;
        axe->trailPosition = axe->auxiliarySprite.position;
        {
            u32 angle = axe->trailStartAngle;
            axe->trailAngle = angle;
        }
        axe->trailSpinRate = 4;
        ActorShadow_Disable(axe->auxiliaryShadow);
        axe->auxiliarySprite.flags = 1;
    }
    {
        s32 angle = axe->trailAngle;
        s32 next = angle - axe->trailSpinRate;
        axe->trailAngle = next;
        if (angle < next)
            Sound_PlaySong(0x169);
    }
    if ((1 & frame) && axe->trailSpinRate <= 19)
        axe->trailSpinRate++;
    ActorPhase_EmitTrailSprite(actor, &axe->trailPosition, axe->trailAngle);
    Axe_UpdateTrailColliders(actor);
    Axe_UpdateAttackColliderPositions(actor);
    if ((axe->auxiliarySprite.flags & 1) && frame > 30) {
        Axe_ReleaseAuxiliarySprite(actor);
        SceneActorTerrain_InstallControl(actor, SceneActorControl_ResetRotation, 0x34);
    }
}

u8 ActorPhase_UpdateReverseFlickerTimer(u32 *, s32, s32, s32);

void Axe_SetAuxiliarySpriteParameters(struct ChaseActor *, const u16 *, u32);

void ActorPhase_UpdateReverseFlickerExit(struct ChaseActor *, s32);

void ActorPhase_UpdateReverseFlicker(struct ChaseActor *actor, s32 frame)
{
    struct Axe *axe = actor->base.context;
    u32 flags;
    if (SceneActorReaction_TakePrepared(actor)) {
        axe->flagsCAB = 0;
        Axe_SetAuxiliarySpriteParameters(actor, axe->originalEffectPalette, 0x13f);
    }
    if (ActorPhase_UpdateReverseFlickerTimer(&flags, frame, 10, 30)) {
        SceneActorTerrain_InstallControl(actor, ActorPhase_UpdateReverseFlickerExit, 0x30);
        goto visible;
    } else if (flags & 1) {
    visible:
        Axe_SetAuxiliarySpriteParameters(actor, (const u16 *)&axe->position, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(actor, axe->originalEffectPalette, 0x13f);
    }
}

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void ActorPhase_UpdateReverseFlickerExit(struct ChaseActor *actor, s32 frame)
{
    struct Axe *axe = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor))
        axe->flagsCAB = 0;
    if (Sprite_BlinkUntilExpired(&axe->auxiliarySprite.flags, frame, 10, 30)) {
        Axe_ReleaseAuxiliarySprite(actor);
        SceneActorTerrain_InstallControl(actor, SceneActorControl_ResetRotation, 0x34);
    }
}

void ActorPhase_WaitBeforeRelocation(struct ChaseActor *, s32);

void ActorPhase_UpdateSpriteCollapse(struct ChaseActor *actor)
{
    struct Axe *axe = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        ActorPhase_GetDrawable(actor)->flags |= 2;
        Axe_SetEffectFlags(4, axe);
        Sound_PlaySong(0x16b);
        SceneActorPlacement_SetFlags(0x1800, &actor->base);
    }
    if ((s8)ActorPhase_GetDrawable(actor)->scaleX > 32) {
        ActorPhase_GetDrawable(actor)->scaleX -= 4;
    } else if ((s8)ActorPhase_GetDrawable(actor)->scaleX > 8) {
        ActorPhase_GetDrawable(actor)->scaleX -= 2;
        ActorPhase_GetDrawable(actor)->scaleY += 4;
        ActorShadow_Disable(actor->base.shadow);
    } else if ((s8)ActorPhase_GetDrawable(actor)->scaleY <= 119) {
        ActorPhase_GetDrawable(actor)->scaleY += 8;
        ActorShadow_Disable(actor->base.shadow);
    } else {
        ActorPhase_GetDrawable(actor)->flags = 1;
        SceneActorTerrain_InstallControl(actor, ActorPhase_WaitBeforeRelocation, 0x24);
        ActorShadow_Disable(actor->base.shadow);
        Axe_SetEffectFlags(0x40, axe);
    }
}

void ActorPhase_SelectDistantWaypoint(struct ChaseActor *, struct ContactPosition *, const void *,
                                      s32);

void ActorPhase_UpdateSpriteRestore(struct ChaseActor *);

void ActorPhase_WaitBeforeRelocation(struct ChaseActor *actor, s32 frame)
{
    struct Axe *axe = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        ActorPhase_GetDrawable(actor)->flags &= ~2;
        {
            struct ContactCollider *collision = &actor->base.colliderB;
            collision->flags |= 4;
        }
        {
            struct ContactCollider *collision = &actor->base.colliderA;
            collision->flags |= 4;
        }
    }
    ActorPhase_GetDrawable(actor)->flags |= 1;
    ActorShadow_Disable(actor->base.shadow);
    if (frame > axe->relocationDelay) {
        ActorPhase_SelectDistantWaypoint(actor, &actor->base.body.position, &gPlayers[0]->position,
                                         0x40000);
        SceneActorTerrain_InstallControl(actor, ActorPhase_UpdateSpriteRestore, 0x25);
    }
}

static inline void clearCollision(struct ContactCollider *p, u32 mask)
{
    p->flags &= ~mask;
}

static inline void clearFlags16(u16 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorPhase_UpdateSpriteRestore(struct ChaseActor *actor)
{
    struct Axe *axe = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        ActorPhase_GetDrawable(actor)->flags = (ActorPhase_GetDrawable(actor)->flags | 2) & ~1;
        ActorPhase_GetDrawable(actor)->angle = 0;
        clearCollision(&actor->base.colliderB, 4);
        clearCollision(&actor->base.colliderA, 4);
        Sound_PlaySong(0x16c);
    }
    if ((s8)ActorPhase_GetDrawable(actor)->scaleY > 112) {
        ActorPhase_GetDrawable(actor)->scaleY -= 8;
    } else if ((s8)ActorPhase_GetDrawable(actor)->scaleY > 64) {
        ActorPhase_GetDrawable(actor)->scaleX += 2;
        ActorPhase_GetDrawable(actor)->scaleY -= 4;
        ActorShadow_Enable(actor->base.shadow);
    } else if ((s8)ActorPhase_GetDrawable(actor)->scaleX <= 63) {
        ActorPhase_GetDrawable(actor)->scaleX += 4;
    } else {
        ActorPhase_GetDrawable(actor)->scaleX = 64;
        ActorPhase_GetDrawable(actor)->scaleY = 64;
        clearFlags(&axe->flagsE4C, 0x44);
        actor->base.stepDone = 1;
        clearFlags16(&actor->base.placementFlags, 0x1800);
    }
}
