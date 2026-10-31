#define gActorLayeredMidAnimations gUnk_0824EDD8
#include "global.h"
struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gActorLayeredMidAnimations[];
void Armor_SetAnimationFromStart(u32, u32);
void Sound_PlaySongChecked(u32);
void ActorController_UpdateLayeredMid(u8 *p)
{
    u8 *s = p + 0xb40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four;
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gActorLayeredMidAnimations[((p[0xb39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xb41] = st;
            {
                u16 *flags = (u16 *)(p + 0xb32);
                u32 current = *flags;
                u32 updated = 2;
                updated |= current;
                *flags = updated;
            }
            {
                u32 v = *(u16 *)(p + 0xb22);
                *(u16 *)(p + 0xb20) = v;
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
            Sound_PlaySongChecked(0xc2);
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
