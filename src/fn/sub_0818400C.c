#define gActorLayeredMidAnimations gUnk_0824EE10
#include "scene_combat_actor.h"
extern const u32 gActorLayeredMidAnimations[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void ActorController_UpdateLayeredMidNoSound(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = &actor->actionPhase;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four;
            Armor_SetAnimationFromStart(
                actor, &gActorLayeredMidAnimations[((actor->heading + 0x60) >> 6) & 3]);
            (*s)++;
            actor->actionReady = st;
            {
                u16 *flags = (u16 *)(p + 0xb32);
                u32 current = *flags;
                u32 updated = 2;
                updated |= current;
                *flags = updated;
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
            break;
        }
        case 1:
            if (actor->animationComplete) {
                *s = 0xff;
                actor->actionReady = st;
                *(u16 *)(s - 14) &= ~2;
            }
            break;
    }
}
