#include "monsters/armor/actor_controller.h"

extern u32 gUnk_0824EED0[];

u32 ActorController_SetAnimation(struct SceneCombatActor *, const void *, u32);

void ActorController_UpdateRetreatMotion(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            ActorController_SetAnimation(actor, &gUnk_0824EED0[((actor->heading + 0x60) >> 6) & 3],
                                         2);
            (*phase)++;
            actor->actionReady = initialPhase;
            actor->animationDuration = 12;
            ActorController_SelectDefenseColliders(actor);
            {
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, 1);
            }
            actor->flags |= 0x20;
        }
        case 1: {
            ActorController_SetMotion(&actor->velocity, (actor->heading + 0x80) & 255, 20);
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
            Armor_SetAnimationFromStart(actor, &gUnk_0824EEE0[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_SelectDefenseColliders(actor);
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
    ActorController_SetMotion(&actor->velocity, actor->motionHeading, 25);
}
