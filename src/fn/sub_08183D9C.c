#define gActorLayeredEndAnimations gUnk_0824EDE8
#include "global.h"
struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gActorLayeredEndAnimations[];
void Armor_SetAnimationFromStart(u32, u32);
void ActorController_UpdateLayeredEnd(u8 *p)
{
    u8 *s = p + 0xb40;
    if (*s == 0) {
        u32 four;
        Armor_SetAnimationFromStart((u32)p,
                                    (u32)&gActorLayeredEndAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
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
        ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
        ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
        ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
        {
            struct ActorRenderFlags *render = (void *)(p + 0x180);
            u32 one = 1;
            render->flags &= ~one;
        }
        *(u16 *)(s - 14) &= ~0x20;
    }
}
