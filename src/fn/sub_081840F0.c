#define gActorLayeredMidRestoreAnimations gUnk_0824EE20
#include "scene_combat_colliders.h"
extern const u32 gActorLayeredMidRestoreAnimations[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Armor_StartPaletteRestoreDelay(void *);
void ActorController_UpdateLayeredMidWithRestoreDelay(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = &actor->actionPhase;
    u32 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            u32 zero;
            u16 *c4;
            st = ((actor->heading + 0x60) >> 6) & 3;
            Armor_SetAnimationFromStart(actor, &gActorLayeredMidRestoreAnimations[st]);
            c4 = (u16 *)(p + 0xc4);
            zero = 0;
            *c4 = 0x10;
            (*s)++;
            actor->actionReady = zero;
            *(u16 *)(p + 0xb32) |= 2;
            ((struct ContactCollider *)(p + 0x238))->flags &= ~four;
            ((struct ContactCollider *)(p + 0x1e4))->flags &= ~four;
            ((struct ContactCollider *)(p + 0x28c))->flags &= ~four;
            {
                struct ContactCollider *l = (struct ContactCollider *)(p + 0x190);
                u16 f = l->flags;
                f |= four;
                l->flags = f;
            }
            {
                struct ActorControllerMotion *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags &= ~one;
            }
            *(u16 *)(s - 14) &= ~0x20;
            Armor_StartPaletteRestoreDelay(p);
            break;
        }
        case 1:
            if (actor->animationComplete) {
                *s = 0xff;
                actor->actionReady = st;
            }
            break;
    }
}
