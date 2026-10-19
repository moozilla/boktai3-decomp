#include "global.h"
struct S {
    u8 f0[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 f3[4];
    u32 w10;
    u8 f5[4];
    u8 b18;
    u8 f6[0xd4 - 0x19];
    u8 d4[1];
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
void StateActor_RunApproachAndFinishSequence(u32 a, struct S *p, s32 a2)
{
    if (p->b9 != 0) {
        s32 animId;
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
            case 4:
                animId = 0;
                break;
            case 5:
                animId = 0;
                break;
            case 6:
                animId = 0;
                break;
            case 7:
                animId = 0;
                break;
            case 8:
                animId = 0;
                break;
            case 9:
                animId = 0;
                break;
            case 10:
                animId = 0;
                break;
            case 11:
                animId = 0;
                break;
            case 12:
                animId = 0;
                break;
            case 13:
                animId = 0;
                break;
            case 14:
                animId = 0;
                break;
            case 15:
                animId = 0;
                break;
            case 16:
                animId = 0;
                break;
            case 17:
                animId = 0;
                break;
            case 18:
                animId = 0;
                break;
            case 19:
                animId = 0;
                break;
            case 20:
                animId = 0;
                break;
            case 21:
                animId = 0;
                break;
            case 22:
                animId = 0;
                break;
            case 23:
                animId = 0;
                break;
            case 24:
                animId = 0;
                break;
            case 25:
                animId = 0;
                break;
            case 26:
                animId = 0;
                break;
            case 27:
                animId = 0;
                break;
            case 28:
                animId = 5;
                break;
            case 29:
                animId = 0;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 2, 0, 4);
        p->b18 = 0;
        p->w10 = 0;
    }
    switch (p->b18) {
        case 0:
            if (DisplayAnimation_TestFinished(p->d4) != 0) {
                s32 animId;
                switch (p->b1) {
                    case 4:
                        animId = 0;
                        break;
                    case 5:
                        animId = 0;
                        break;
                    case 6:
                        animId = 0;
                        break;
                    case 7:
                        animId = 0;
                        break;
                    case 8:
                        animId = 0;
                        break;
                    case 9:
                        animId = 0;
                        break;
                    case 10:
                        animId = 0;
                        break;
                    case 11:
                        animId = 0;
                        break;
                    case 12:
                        animId = 0;
                        break;
                    case 13:
                        animId = 0;
                        break;
                    case 14:
                        animId = 0;
                        break;
                    case 15:
                        animId = 0;
                        break;
                    case 16:
                        animId = 0;
                        break;
                    case 17:
                        animId = 0;
                        break;
                    case 18:
                        animId = 0;
                        break;
                    case 19:
                        animId = 0;
                        break;
                    case 20:
                        animId = 0;
                        break;
                    case 21:
                        animId = 0;
                        break;
                    case 22:
                        animId = 0;
                        break;
                    case 23:
                        animId = 0;
                        break;
                    case 24:
                        animId = 0;
                        break;
                    case 25:
                        animId = 0;
                        break;
                    case 26:
                        animId = 0;
                        break;
                    case 27:
                        animId = 0;
                        break;
                    case 28:
                        animId = 6;
                        break;
                    case 29:
                        animId = 0;
                        break;
                    default:
                        animId = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, animId, 2, 0, 4);
                p->b18++;
                p->w10 = 0;
            }
            break;
        case 1:
            if (a2 > 0xa) {
                s32 animId;
                switch (p->b1) {
                    case 4:
                        animId = 0;
                        break;
                    case 5:
                        animId = 0;
                        break;
                    case 6:
                        animId = 0;
                        break;
                    case 7:
                        animId = 0;
                        break;
                    case 8:
                        animId = 0;
                        break;
                    case 9:
                        animId = 0;
                        break;
                    case 10:
                        animId = 0;
                        break;
                    case 11:
                        animId = 0;
                        break;
                    case 12:
                        animId = 0;
                        break;
                    case 13:
                        animId = 0;
                        break;
                    case 14:
                        animId = 0;
                        break;
                    case 15:
                        animId = 0;
                        break;
                    case 16:
                        animId = 0;
                        break;
                    case 17:
                        animId = 0;
                        break;
                    case 18:
                        animId = 0;
                        break;
                    case 19:
                        animId = 0;
                        break;
                    case 20:
                        animId = 0;
                        break;
                    case 21:
                        animId = 0;
                        break;
                    case 22:
                        animId = 0;
                        break;
                    case 23:
                        animId = 0;
                        break;
                    case 24:
                        animId = 0;
                        break;
                    case 25:
                        animId = 0;
                        break;
                    case 26:
                        animId = 0;
                        break;
                    case 27:
                        animId = 0;
                        break;
                    case 28:
                        animId = 5;
                        break;
                    case 29:
                        animId = 0;
                        break;
                    default:
                        animId = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, animId, 2, 0, 4);
                p->b18++;
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(p->d4) != 0) {
                p->b7 = 1;
            }
            break;
    }
}
