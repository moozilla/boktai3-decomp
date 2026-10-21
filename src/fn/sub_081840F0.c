// MATCHING_CONCESSION: register-pinning
#define gActorLayeredMidRestoreAnimations gUnk_0824EE20
#include "global.h"
struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
extern const u32 gActorLayeredMidRestoreAnimations[];
void Armor_SetAnimationFromStart(u32, u32);
void Armor_StartPaletteRestoreDelay(void *);
void ActorController_UpdateLayeredMidWithRestoreDelay(u8 *p)
{
    u8 *s = p + 0xb40;
    u32 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            register u32 zero asm("r2");
            u16 *c4;
            st = ((p[0xb39] + 0x60) >> 6) & 3;
            Armor_SetAnimationFromStart((u32)p, (u32)&gActorLayeredMidRestoreAnimations[st]);
            c4 = (u16 *)(p + 0xc4);
            zero = 0;
            asm("" : "+r"(zero));
            *c4 = 0x10;
            (*s)++;
            p[0xb41] = zero;
            *(u16 *)(p + 0xb32) |= 2;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
            {
                struct ActorAnimationLayer *l = (struct ActorAnimationLayer *)(p + 0x190);
                u16 f = l->flags;
                f |= four;
                l->flags = f;
            }
            {
                struct ActorRenderFlags *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags &= ~one;
            }
            *(u16 *)(s - 14) &= ~0x20;
            Armor_StartPaletteRestoreDelay(p);
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
