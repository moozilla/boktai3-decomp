#include "global.h"
#define StateActor_RunEntryAnimSequence sub_081C7F00
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 f3[6];
    u32 w10;
    u8 f4[4];
    u8 b18;
    u8 f5[0xd4 - 0x19];
    u8 d4[1];
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
void sub_081C7F00(u32 a, struct S *p, s32 a2)
{
    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        Actor_PlayAnimationEntry(p, 0, 2, 0, 4);
        p->b18 = 0;
        p->w10 = 0;
    }
    switch (p->b18) {
        case 0:
        {
            u32 r1;
            if (p->w10 <= 10)
                break;
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
                case 23:
                case 24:
                case 25:
                case 26:
                case 28:
                case 29:
                    r1 = 0;
                    break;
                case 27:
                    r1 = 0x15;
                    break;
                default:
                    r1 = 0;
                    break;
            }
            Actor_PlayAnimationEntry(p, r1, 2, 0, 4);
            p->b18++;
            break;
        }
        case 1:
            if (DisplayAnimation_TestFinished(p->d4) != 0) {
                u32 zero = 0;
                Actor_PlayAnimationEntry(p, zero, 2, 0, 4);
                p->b18++;
                p->w10 = zero;
            }
            break;
        case 2:
            if (a2 > 10) {
                p->b7 = 1;
            }
            break;
    }
}
