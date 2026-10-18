// Callee aliases below resolve against the checker ELF predating symbol promotion.
#define gActorPaletteTransitionAnimations gUnk_0824ED98
#include "global.h"

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};

extern u32 gActorPaletteTransitionAnimations[];
void Armor_StartPaletteTransition(void *);
void Armor_SetAnimationFromStart(u32 a, u32 b);

void ActorController_UpdatePaletteAnimationTransition(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gActorPaletteTransitionAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
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
            Armor_StartPaletteTransition(p);
            break;
        }
        case 1:
            if (p[0xB43] != 0) {
                *s = 0xFF;
                p[0xB41] = st;
            }
            break;
    }
}
