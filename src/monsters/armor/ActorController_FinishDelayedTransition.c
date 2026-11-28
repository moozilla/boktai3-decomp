#define gActorControllerDelayedTransitionAnimations gUnk_0824EEC0
#include "monsters/armor/scene_combat_colliders.h"

extern u32 gActorControllerDelayedTransitionAnimations[];

void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);

void ActorController_FinishDelayedTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *state = p + 0xB40;
    if (*state == 0) {
        Armor_SetAnimationFromStart(
            actor,
            &gActorControllerDelayedTransitionAnimations[((actor->heading + 0x60) >> 6) & 3]);
        *state = 0xFF;
        actor->actionReady = 1;
        actor->animationDuration = 0x5B;
        SceneCombatActor_SelectFirstCollider((struct SceneCombatActor *)p);
        actor->flags &= ~0x20;
    }
}
