// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_control_phase.h"
void Sound_PlaySong(u32);
void ActorShadow_Disable(void *);
void ActorPhase_EmitTrailSprite(void *, void *, u32);
void Axe_UpdateTrailColliders(void *);
void Axe_UpdateAttackColliderPositions(void *);
void Axe_ReleaseAuxiliarySprite(void *);
static inline void SetState(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
// confidence=low: spins the auxiliary axe after damage and releases it after 30 frames.
void Axe_UpdateDamageSpin(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    u8 *context = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u8 *state = actor->context;
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->flags2CC;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 0;
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        Sound_PlaySong(318);
        *(u16 *)((u8 *)actor + 0x178) = 0;
        if (actor->drawableKind == 0) {
            u32 *flags = actor->drawable;
            u32 mask = ~1;
            *flags &= mask;
        } else {
            u8 *tile = (u8 *)actor->drawable + 0x20;
            u32 mask = ~1;
            *(u32 *)(tile + 8) &= mask;
        }
        SetState(actor, 3, 0, 0);
        actor->fieldAA = 0;
        if (state[0xA3C]) {
            u32 *flags = (void *)(state + 0xA38);
            *flags |= 2;
            state[0xCAB] = 0;
            state[0xA35] = 0;
            *(struct ContactPosition *)(state + 0xA24) = *(struct ContactPosition *)(state + 0xA54);
            {
                u32 angle = state[0xCA8];
                state[0xA34] = angle;
            }
            state[0xA36] = 4;
            ActorShadow_Disable(state + 0xCAC);
            *flags = 1;
        }
        {
            u32 off = 0xED6;
            register u8 *q asm("r0");
            asm("" : "+r"(off));
            q = state + off;
            *q = 1;
            off -= 2;
            q = state + off;
            *(u16 *)q = 0;
        }
    }
    if (*(u16 *)(context + 0xED4)) {
        --*(u16 *)(context + 0xED4);
        {
            u32 mask = 32;
            asm("" : "+r"(mask));
            actor->flags2D6 = mask | *(u16 *)((u8 *)actor + 0x2D6);
        }
    }
    if (context[0xA3C]) {
        u32 previous = context[0xA34];
        s32 next = previous - context[0xA36];
        context[0xA34] = next;
        if ((s32)previous < next)
            Sound_PlaySong(361);
        if ((frame & 1) && context[0xA36] <= 19)
            context[0xA36]++;
        ActorPhase_EmitTrailSprite(actor, context + 0xA24, context[0xA34]);
        Axe_UpdateTrailColliders(actor);
        Axe_UpdateAttackColliderPositions(actor);
        if (frame > 30)
            Axe_ReleaseAuxiliarySprite(actor);
    }
}
