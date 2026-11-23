#define gActorControllerDelayedTransitionAnimations gUnk_0824EEC0
#include "scene_combat_colliders.h"

extern u32 gActorControllerDelayedTransitionAnimations[];

void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);

void ActorController_FinishDelayedTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *state = p + 0xB40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart(
            actor, &gActorControllerDelayedTransitionAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
        *state = 0xFF;
        p[0xB41] = 1;
        actor->animationDuration = 0x5B;
        ((struct ContactCollider *)(p + 0x238))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x1E4))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x28C))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x190))->flags |= four;
        {
            u32 one = 1;
            ((struct ActorControllerMotion *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(p + 0xB32) &= ~0x20;
    }
}
