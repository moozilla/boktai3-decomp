// Callee aliases below resolve against the checker ELF predating symbol promotion.
#define gActorLayeredFollowupAnimations                  gUnk_0824EDB8
#define gActorLayeredStartAnimations                     gUnk_0824EDA8
#define ActorController_UpdateLayeredAnimationTransition sub_08183B54
#include "global.h"
struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gActorLayeredStartAnimations[];
extern const u32 gActorLayeredFollowupAnimations[];
void Sound_PlaySongChecked(u32);
void Armor_SetAnimationFromStart(u32, u32);
void sub_08183B54(u8 *p)
{
    u8 *s = p + 0xb40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gActorLayeredStartAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xb41] = st;
            Sound_PlaySongChecked(0xc1);
            ((struct ActorAnimationLayer *)(p + 0x238))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                struct ActorRenderFlags *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags |= one;
            }
            *(u16 *)(p + 0xb32) &= ~0x20;
            break;
        }
        case 1:
            if (p[0xb43]) {
                Armor_SetAnimationFromStart(
                    (u32)p, (u32)&gActorLayeredFollowupAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
                *s = 0xff;
                p[0xb41] = st;
            }
            break;
    }
}
