#include "actor_phase.h"
#include "player.h"

static inline u8 TakePhaseEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}

static inline void ClearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

static inline void SetPhase(struct SceneActorRegistryResetView *actor, void *next, u32 id)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = id;
    actor->target = next;
    actor->field2C8 = 0;
    ClearFlags(&actor->flags114, 1);
    actor->word104 = 0;
}

void ActorPhase_UpdateAcceleratingTrail(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct ActorPhaseContext *context = actor->context;
    if (TakePhaseEntry(actor)) {
        context->trailSprite.flags |= 2;
        context->animationState = 0;
        context->trailMode = 0;
        context->trailOrigin = *(struct ContactPosition *)&context->trailSprite.x;
        {
            u32 angle = context->initialAngle;
            context->trailAngle = angle;
        }
        context->angularSpeed = 4;
        ActorShadow_Disable(context->shadow);
        context->trailSprite.flags = 1;
    }
    {
        s32 angle = context->trailAngle;
        s32 next = angle - context->angularSpeed;
        context->trailAngle = next;
        if (angle < next)
            Sound_PlaySong(0x169);
    }
    if ((1 & frame) && context->angularSpeed <= 19)
        context->angularSpeed++;
    ActorPhase_EmitTrailSprite(actor, &context->trailOrigin, context->trailAngle);
    Axe_UpdateTrailColliders(actor);
    Axe_UpdateAttackColliderPositions(actor);
    if ((context->trailSprite.flags & 1) && frame > 30) {
        Axe_ReleaseAuxiliarySprite(actor);
        {
            void *next = SceneActorControl_ResetRotation;
            u32 id = 0x34;
            SetPhase(actor, next, id);
        }
    }
}

void ActorPhase_UpdateReverseFlicker(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct ActorPhaseContext *context = actor->context;
    u32 flags;
    if (TakePhaseEntry(actor)) {
        context->animationState = 0;
        Axe_SetAuxiliarySpriteParameters(actor, context->flickerAnimation, 0x13f);
    }
    if (ActorPhase_UpdateReverseFlickerTimer(&flags, frame, 10, 30)) {
        void *next = ActorPhase_UpdateReverseFlickerExit;
        u32 id = 0x30;
        SetPhase(actor, next, id);
        goto visible;
    } else if (flags & 1) {
    visible:
        Axe_SetAuxiliarySpriteParameters(actor, context->visibleAnimation, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(actor, context->flickerAnimation, 0x13f);
    }
}

void ActorPhase_UpdateReverseFlickerExit(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct ActorPhaseContext *context = actor->context;
    if (TakePhaseEntry(actor))
        context->animationState = 0;
    if (Sprite_BlinkUntilExpired(&context->trailSprite, frame, 10, 30)) {
        Axe_ReleaseAuxiliarySprite(actor);
        {
            void *next = SceneActorControl_ResetRotation;
            u32 id = 0x34;
            SetPhase(actor, next, id);
        }
    }
}

void ActorPhase_UpdateSpriteCollapse(struct SceneActorRegistryResetView *actor)
{
    struct ActorPhaseContext *context = actor->context;
    if (TakePhaseEntry(actor)) {
        ((struct ActorSprite *)actor->drawable)->flags |= 2;
        {
            u32 mask = 4;
            context->flags |= mask;
        }
        Sound_PlaySong(0x16b);
        {
            u32 mask = 0x1800;
            actor->flags2C0 = mask | actor->flags2C0;
        }
    }
    if (((struct ActorSprite *)actor->drawable)->scaleX > 32) {
        ((struct ActorSprite *)actor->drawable)->scaleX -= 4;
    } else if (((struct ActorSprite *)actor->drawable)->scaleX > 8) {
        ((struct ActorSprite *)actor->drawable)->scaleX -= 2;
        ((struct ActorSprite *)actor->drawable)->scaleY += 4;
        ActorShadow_Disable(actor->shadow);
    } else if (((struct ActorSprite *)actor->drawable)->scaleY <= 119) {
        ((struct ActorSprite *)actor->drawable)->scaleY += 8;
        ActorShadow_Disable(actor->shadow);
    } else {
        void *next;
        u32 id;
        ((struct ActorSprite *)actor->drawable)->flags = 1;
        next = ActorPhase_WaitBeforeRelocation;
        id = 0x24;
        SetPhase(actor, next, id);
        ActorShadow_Disable(actor->shadow);
        {
            u32 mask = 0x40;
            context->flags |= mask;
        }
    }
}

void ActorPhase_WaitBeforeRelocation(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct ActorPhaseContext *context = actor->context;
    if (TakePhaseEntry(actor)) {
        ((struct ActorSprite *)actor->drawable)->flags &= ~2;
        {
            struct ContactCollider *collision = &actor->colliderB;
            collision->flags |= 4;
        }
        {
            struct ContactCollider *collision = &actor->colliderA;
            collision->flags |= 4;
        }
    }
    ((struct ActorSprite *)actor->drawable)->flags |= 1;
    ActorShadow_Disable(actor->shadow);
    if (frame > context->relocationDelay) {
        ActorPhase_SelectDistantWaypoint(actor, &actor->body.position, &gPlayers[0]->position,
                                         0x40000);
        {
            void *next = ActorPhase_UpdateSpriteRestore;
            u32 id = 0x25;
            SetPhase(actor, next, id);
        }
    }
}

static inline void ClearCollisionFlags(struct ContactCollider *p, u32 mask)
{
    p->flags &= ~mask;
}

static inline void ClearHalfwordFlags(u16 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorPhase_UpdateSpriteRestore(struct SceneActorRegistryResetView *actor)
{
    struct ActorPhaseContext *context = actor->context;
    if (TakePhaseEntry(actor)) {
        ((struct ActorSprite *)actor->drawable)->flags =
            (((struct ActorSprite *)actor->drawable)->flags | 2) & ~1;
        ((struct ActorSprite *)actor->drawable)->unk04[2] = 0;
        ClearCollisionFlags(&actor->colliderB, 4);
        ClearCollisionFlags(&actor->colliderA, 4);
        Sound_PlaySong(0x16c);
    }
    if (((struct ActorSprite *)actor->drawable)->scaleY > 112) {
        ((struct ActorSprite *)actor->drawable)->scaleY -= 8;
    } else if (((struct ActorSprite *)actor->drawable)->scaleY > 64) {
        ((struct ActorSprite *)actor->drawable)->scaleX += 2;
        ((struct ActorSprite *)actor->drawable)->scaleY -= 4;
        ActorShadow_Enable(actor->shadow);
    } else if (((struct ActorSprite *)actor->drawable)->scaleX <= 63) {
        ((struct ActorSprite *)actor->drawable)->scaleX += 4;
    } else {
        ((struct ActorSprite *)actor->drawable)->scaleX = 64;
        ((struct ActorSprite *)actor->drawable)->scaleY = 64;
        ClearFlags(&context->flags, 0x44);
        actor->field2B0 = 1;
        ClearHalfwordFlags(&actor->flags2C0, 0x1800);
    }
}
