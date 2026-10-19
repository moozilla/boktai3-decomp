#include "global.h"


void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);

void StateActor_UpdateFacingAnimation(u32 a, u8 *b, u32 c)
{
    u32 anim;
    s16 *p = (s16 *)(b + 0x20);
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        if (p[3] == 1) {
            switch (b[1]) {
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
                    anim = 0;
                    break;
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
                case 27:
                case 28:
                    anim = 0;
                    break;
                case 29:
                    anim = 6;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(b, anim, 1, 1, 4);
        } else {
            switch (b[1]) {
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
                    anim = 0;
                    break;
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
                case 27:
                case 28:
                    anim = 0;
                    break;
                case 29:
                    anim = 0xd;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(b, anim, 0, 1, 4);
        }
        {
            u32 zero = 0;
            b[0x18] = zero;
            *(u32 *)(b + 0x10) = zero;
        }
    } else {
        if (p[3] == 1) {
            switch (b[1]) {
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
                    anim = 0;
                    break;
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
                case 27:
                case 28:
                    anim = 0;
                    break;
                case 29:
                    anim = 6;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(b, anim, 1, 1, 1);
        } else {
            switch (b[1]) {
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
                    anim = 0;
                    break;
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
                case 27:
                case 28:
                    anim = 0;
                    break;
                case 29:
                    anim = 0xd;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(b, anim, 0, 1, 1);
        }
    }
    if (DisplayAnimation_TestFinished(b + 0xd4))
        b[7] = 1;
}
