#define gActorLayeredEndAnimations gUnk_0824EDE8
#include "monsters/armor/scene_combat_colliders.h"
extern const u32 gActorLayeredEndAnimations[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void ActorController_UpdateLayeredEnd(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = p + 0xb40;
    if (*s == 0) {
        Armor_SetAnimationFromStart(actor,
                                    &gActorLayeredEndAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
        *s = 0xff;
        p[0xb41] = 1;
        actor->flags |= 2;
        SceneCombatActor_SelectFirstCollider((struct SceneCombatActor *)p);
        *(u16 *)(s - 14) &= ~0x20;
    }
}
