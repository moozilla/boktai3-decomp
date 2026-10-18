// Callee aliases below resolve against the checker ELF predating symbol promotion.
#define gActorFadeFollowupAnimations                  gUnk_0824ED68
#define gActorFadeStartAnimations                     gUnk_0824ED58
#define ActorController_UpdateFadeAnimationTransition sub_08183874
#include "global.h"

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};

extern u32 gActorFadeStartAnimations[];
extern u32 gActorFadeFollowupAnimations[];
void Armor_SetAnimationFromStart(u32 a, u32 b);
void Sound_PlaySongChecked(u32 a);

void Armor_StartFastWhiteFade(void *);

void sub_08183874(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gActorFadeStartAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ActorRenderFlags *)(p + 0x180))->flags &= ~one;
            }
            *(u16 *)(p + 0xB32) &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (p[0xB43] != 0) {
                Armor_SetAnimationFromStart(
                    (u32)p, (u32)&gActorFadeFollowupAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    p[0xB42] = zero;
                }
                Armor_StartFastWhiteFade(p);
            } else
                break;
        case 2:
            if (p[0xB42] > 7) {
                p[0xB40] = 0xFF;
                p[0xB41] = 1;
            }
            break;
    }
}
