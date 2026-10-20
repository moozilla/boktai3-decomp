#define gActorBounceAnimations gUnk_0824EE30
#include "global.h"
struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gActorBounceAnimations[];
void Armor_SetAnimationFromStart(u32, u32);
void SceneCombatActor_SpawnBouncingParticle(void *);
void ActorController_UpdateBounceParticle(u8 *p)
{
    u8 *s = p + 0xb40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four;
            Armor_SetAnimationFromStart((u32)p,
                                        (u32)&gActorBounceAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
            four = 0xb32;
            (*s)++;
            p[0xb41] = st;
            SceneCombatActor_SpawnBouncingParticle(p);
            *(u16 *)(p + four) &= ~1;
            four = 4;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                struct ActorRenderFlags *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags &= ~one;
            }
            *(u16 *)(p + 0xb32) &= ~(long long)0x20;
            break;
        }
        case 1:
            if (p[0xb43]) {
                *s = 0xff;
                p[0xb41] = st;
            }
            break;
    }
}
