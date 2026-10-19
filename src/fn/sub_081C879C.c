#include "global.h"
#define StateActor_StartAndMaybeFinishAnim sub_081C879C
struct S {
    u8 f0[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 f3[0x18 - 0xa];
    u8 b18;
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
void sub_081C879C(u32 a, struct S *p, s32 a2)
{
    u8 *base20 = (u8 *)p + 0x20;
    if (p->b9 != 0) {
        u32 animId;
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0xe;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 2, 0, 6);
        {
            u32 zero = 0;
            u16 v = 0x96 << 1;
            *(u16 *)(base20 + 2) = v;
            p->b18 = zero;
        }
    }
    if (a2 != 1)
        return;
    {
        u32 animId;
        switch (p->b1) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0xe;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 0, 0, 3);
    }
    p->b7 = 1;
}
