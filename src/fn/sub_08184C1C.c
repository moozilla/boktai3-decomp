#define gActorStopAnimations gUnk_0824EEF0
// Names describe observed behavior; the action interpretation is tentative.
#include "global.h"

struct ActorAnimationFlags {
    u16 unk0, unk2, unk4, flags;
};
struct ActorStateFlags {
    u32 flags : 16;
};
struct ActorMotionFlags {
    u32 unk0, flags;
};
struct ActorMotionVector {
    s16 x, y, z;
};
extern u32 gUnk_0824EEF0[];
void Armor_SetAnimationFromStart(u32, u32);
void ActorPalette_DisableCallbacks(u8 *);

void ActorController_CompleteMotionTransition(u8 *p)
{
    u8 *state = p + 0xB40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EEF0[((p[0xB39] + 0x60) >> 6) & 3]);
        *state = 255;
        p[0xB41] = 1;
        ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x190))->flags =
            four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
        {
            u32 one = 1;
            ((struct ActorMotionFlags *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(p + 0xB32) &= 0xFFDF;

        switch (p[0xB38]) {
            case 3:
            case 5:
            case 7:
            case 9:
            case 13:
            case 15:
                ActorPalette_DisableCallbacks(p);
                break;
        }
    }
}
