#define gActorBounceAnimations gUnk_0824EE30
#include "monsters/armor/scene_combat_actor.h"
extern const u32 gActorBounceAnimations[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void SceneCombatActor_SpawnBouncingParticle(void *);
void ActorController_UpdateBounceParticle(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = &actor->actionPhase;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four;
            Armor_SetAnimationFromStart(
                actor, &gActorBounceAnimations[((actor->heading + 0x60) >> 6) & 3]);
            four = 0xb32;
            (*s)++;
            actor->actionReady = st;
            SceneCombatActor_SpawnBouncingParticle(p);
            *(u16 *)(p + four) &= ~1;
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
            *(u16 *)(p + 0xb32) &= ~(long long)0x20;
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
