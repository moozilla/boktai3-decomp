#define gActorLayeredEndAnimations gUnk_0824EDE8
#include "scene_combat_colliders.h"
extern const u32 gActorLayeredEndAnimations[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void ActorController_UpdateLayeredEnd(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = p + 0xb40;
    if (*s == 0) {
        u32 four;
        Armor_SetAnimationFromStart(actor,
                                    &gActorLayeredEndAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
        *s = 0xff;
        p[0xb41] = 1;
        {
            u16 *f = (u16 *)(p + 0xb32);
            u32 v = *f;
            u32 two = 2;
            two |= v;
            *f = two;
        }
        four = 4;
        ((struct ContactCollider *)(p + 0x238))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x1e4))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x28c))->flags &= ~four;
        ((struct ContactCollider *)(p + 0x190))->flags |= four;
        {
            struct ActorControllerMotion *render = (void *)(p + 0x180);
            u32 one = 1;
            render->flags &= ~one;
        }
        *(u16 *)(s - 14) &= ~0x20;
    }
}
