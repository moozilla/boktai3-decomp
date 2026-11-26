#include "bosses/sabata/sabata_actor_state.h"
#include "engine/word_flags.h"

static inline void hide(struct AnimSprite *a)
{
    a->flags |= 1;
}
static inline void fade(struct AnimSprite *a)
{
    a->flags = (a->flags & ~1) | 0x200;
}
static inline void unfade(struct AnimSprite *a)
{
    a->flags &= ~0x201;
}
void SceneActors_UpdateEffects(void *);
void SceneActors_UpdateConvergingParticles(void *);
u32 ActorExtension_ResetState(void *);
void Collider_SetPosition(void *, void *, u32);
void SpriteComponent_Resume(void *);
void SpriteComponent_Suspend(void *);
void ProximityVolume_DispatchUpdate(void *);
s32 SabataPosition_SaveSelectedPoint(void *);
void SabataFrost_StartEmission(void *, void *);
void SabataFrost_StopEmission(void *);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void Camera_RequestShake(s32);
s32 ActorProperty_GetConstant34(void *);
void Camera_SetOffset(s32, void *);
void SceneActors_EmitRandomBursts(void *);
void Sabata_UpdateSequencePalette(void *);

void Sabata_UpdatePresentation(struct SabataActorState *p)
{
    struct SabataActorState *q = p;
    struct SceneActorBody *sprite = &p->body;
    u32 *flags;
    struct ContactCollider *collider;
    u8 *grounded;
    SceneActors_UpdateEffects(p);
    SceneActors_UpdateConvergingParticles(p);
    if (p->controlPhase == 10) {
        if (q->controlPhaseTimer > 0)
            q->controlPhaseTimer--;
    } else
        q->controlPhaseTimer = 0;
    {
        struct SabataActorState *a = q;
        u32 v = a->contactHoldoff;
        flags = &p->flags;
        if (v) {
            WordFlags_Set(4, flags);
            a->contactHoldoff--;
        }
    }
    collider = &q->bodyCollider;
    if (WordFlags_Test(flags, 4)) {
        collider->flags |= 4;
        ActorExtension_ResetState(q);
    } else {
        ContactCollider_ClearFlags(collider, 6);
        Collider_SetPosition(collider, &q->graphics.position, 0);
        {
            u32 direction = sprite->direction;
            q->bodyCollider.direction = direction;
        }
    }
    collider = &q->proximityCollider;
    if (WordFlags_Test(flags, 0x400000)) {
        collider->flags |= 4;
    } else {
        ContactCollider_ClearFlags(collider, 4);
        Collider_SetPosition(collider, &q->graphics.position, 0);
        {
            u32 direction = sprite->direction;
            q->proximityCollider.direction = direction;
        }
    }
    if (WordFlags_Test(flags, 0x100000))
        SpriteComponent_Resume(q->effect);
    else
        SpriteComponent_Suspend(q->effect);
    ProximityVolume_DispatchUpdate(q->proximityVolume);
    q->transitionBlocked = SabataPosition_SaveSelectedPoint(q);
    if (WordFlags_Test(flags, 0x200000))
        SabataFrost_StartEmission(q->frostEmitter, &p->body.position);
    else
        SabataFrost_StopEmission(q->frostEmitter);
    if (WordFlags_Test(flags, 0x10)) {
        hide(&q->graphics);
        ActorShadow_Disable(q->shadow);
        CameraTracking_ClearRequests(&q->camera, 1);
        ContactCollider_ClearFlags(&q->bodyCollider, 8);
        grounded = &p->special;
    } else if (WordFlags_Test(flags, 0x40000)) {
        fade(&q->graphics);
        ActorShadow_Disable(q->shadow);
        CameraTracking_ClearRequests(&q->camera, 1);
        ContactCollider_ClearFlags(&q->bodyCollider, 8);
        grounded = &p->special;
    } else {
        unfade(&q->graphics);
        {
            struct SabataActorState *shadowActor = q;
            u32 val = p->special;
            grounded = &p->special;
            if (val) {
                ActorShadow_Enable(shadowActor->shadow);
                CameraTracking_ClearRequests(&q->camera, 1);
            } else {
                ActorShadow_Enable(shadowActor->shadow);
                CameraTracking_ClearRequests(&q->camera, 1);
            }
        }
        if (WordFlags_Test(flags, 0x20)) {
            struct ContactCollider *body = &q->bodyCollider;
            body->flags |= 8;
        } else
            ContactCollider_ClearFlags(&q->bodyCollider, 8);
    }
    if (q->shakeTimer) {
        u32 n = q->shakeTimer;
        Camera_RequestShake((s32)(n * n) >> 2);
        q->shakeTimer--;
    }
    if (q->jitterTicks) {
        q->jitterTicks--;
        WordFlags_Set(0x20000, flags);
    }
    if (q->paletteRestoreTimer > 0) {
        q->paletteRestoreTimer--;
        if (q->paletteRestoreTimer <= 0) {
            s32 k = ActorProperty_GetConstant34(q);
            if (k == 0)
                q->graphics.palette = q->effectPalettes[1];
            else if (k == 1)
                q->graphics.palette = q->effectPalettes[2];
            else if (k == 2)
                q->graphics.palette = q->paletteData;
            else
                q->graphics.palette = q->effectPalettes[0];
            q->paletteRestoreTimer = 0;
        }
    } else if (q->paletteMode)
        Sabata_UpdateSequencePalette(q);
    if (!*grounded) {
        if (p->controlPhase == 0xb)
            Camera_SetOffset(0xf, &q->graphics.position);
        else
            CameraTracking_Update(&q->camera);
    } else
        Camera_SetOffset(0x1e, &q->graphics.position);
    if (*grounded && q->emitBursts && !(p->presentationFrames & 0xf))
        SceneActors_EmitRandomBursts(q);
    p->rewardMask = 0;
    p->contactAttributes = 0;
    p->presentationFrames++;
}
