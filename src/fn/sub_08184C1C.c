#define gActorStopAnimations gUnk_0824EEF0
// Names describe observed behavior; the action interpretation is tentative.
#include "scene_combat_colliders.h"

extern u32 gUnk_0824EEF0[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void ActorPalette_DisableCallbacks(u8 *);

void ActorController_CompleteMotionTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *state = p + 0xB40;
    if (*state == 0) {
        Armor_SetAnimationFromStart(actor, &gUnk_0824EEF0[((p[0xB39] + 0x60) >> 6) & 3]);
        *state = 255;
        p[0xB41] = 1;
        SceneCombatActor_SelectFirstCollider((struct SceneCombatActor *)p);
        *(u16 *)(p + 0xB32) &= 0xFFDF;

        switch (p[0xB38]) {
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
