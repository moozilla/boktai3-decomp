#include "global.h"
#define StateActor_ApproachTargetPose sub_081C84CC
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 f3[4];
    u16 h0e;
    u8 f4[0x18 - 0x10];
    u8 b18;
    u8 f5[0x54 - 0x19];
    s16 h54;
    s16 h56;
    s16 h58;
    u8 f6[0x68 - 0x5a];
    s16 h68;
    s16 h6a;
    s16 h6c;
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 ScalarApproach_StepSixteenth(s32, s32);
void sub_081C84CC(u32 a, struct S *p)
{
    {
        u16 mask = 0x400;
        p->h0e |= mask;
    }
    if (p->b9 != 0) {
        u32 r1;
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
                r1 = 0;
                break;
            case 27:
                r1 = 0x10;
                break;
            default:
                r1 = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r1, 1, 1, 4);
        p->b18 = 0;
    }
    switch (p->b18) {
        case 0:
            p->h68 = ScalarApproach_StepSixteenth(p->h68, p->h54);
            p->h6a = ScalarApproach_StepSixteenth(p->h6a, p->h56);
            p->h6c = ScalarApproach_StepSixteenth(p->h6c, p->h58);
            if (*(u32 *)&p->h68 == *(u32 *)&p->h54 && p->h6c == p->h58) {
                p->b18++;
            }
            break;
        case 1:
            p->b7 = 1;
            break;
    }
}
