#define gActorStopAnimations gUnk_0824EEF0
// Names describe observed behavior; the action interpretation is tentative.
#include "monsters/armor/scene_combat_colliders.h"

extern u32 gUnk_0824EEF0[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void ActorPalette_DisableCallbacks(u8 *);

void ActorController_CompleteMotionTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *state = p + 0xB40;
    if (*state == 0) {
        Armor_SetAnimationFromStart(actor, &gUnk_0824EEF0[((actor->heading + 0x60) >> 6) & 3]);
        *state = 255;
        actor->actionReady = 1;
        SceneCombatActor_SelectFirstCollider((struct SceneCombatActor *)p);
        actor->flags &= ~0x20;

        switch (actor->kind) {
            case 3:
            case 5:
            case 7:
            case 9:
            case 13:
            case 15:
                ActorPalette_DisableCallbacks(p);
                break;
        }
    }
}
