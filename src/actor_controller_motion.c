#include "actor_controller.h"

extern u32 gUnk_0824EED0[];

u32 ActorController_SetAnimation(struct SceneCombatActor *, const void *, u32);

static inline s32 ActorController_TruncateMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline void ActorController_SetMotion(s16 *velocity, s32 angle, s32 speed)
{
    s32 x = ActorController_TruncateMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    velocity[0] = x;
    velocity[1] = zero;
    velocity[2] = ActorController_TruncateMotion(gUnk_086149C4[angle] * speed);
}

void ActorController_UpdateRetreatMotion(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            ActorController_SetAnimation(actor, &gUnk_0824EED0[((actor->heading + 0x60) >> 6) & 3],
                                         2);
            (*phase)++;
            actor->actionReady = initialPhase;
            actor->animationDuration = 12;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags |= 0x20;
        }
        case 1: {
            ActorController_SetMotion(actor->velocity, (actor->heading + 0x80) & 255, 20);
            if (actor->animationComplete) {
                actor->actionPhase = 0xFF;
                actor->actionReady = 1;
            }
            break;
        }
    }
}

extern u32 gUnk_0824EEE0[];

void ActorController_UpdateTimedMotion(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, &gUnk_0824EEE0[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                struct ActorControllerMotion *motion = &actor->colliderGroup.motion;
                u32 one = 1;
                motion->flags |= one;
            }
            actor->flags |= 0x20;
        } break;
        case 1:
            if (actor->actionCounter > 9) {
                *phase = 0xFF;
                actor->actionReady = initialPhase;
            }
            break;
    }
    ActorController_SetMotion(actor->velocity, actor->motionHeading, 25);
}
