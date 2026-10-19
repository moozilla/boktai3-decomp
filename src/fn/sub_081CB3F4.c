#include "global.h"

#define StateActor_UpdateKind29AttackEffect sub_081CB3F4

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
u8 *TimedSpriteEffect_Create(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5,
                             s32 arg6, s32 arg7, u32 arg8);

struct VecCB3F4 {
    s16 x, y, z, pad;
};

void sub_081CB3F4(u32 a, u8 *b, u32 c)
{
    u32 anim;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
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
                anim = 4;
                break;
            default:
                anim = 0;
                break;
        }
        Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
        {
            u32 zero = 0;
            b[0x18] = zero;
            *(u32 *)(b + 0x10) = zero;
        }
    }
    switch (b[0x18]) {
        case 0:
            if (*(u32 *)(b + 0x10) > 0x3c) {
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
                        anim = 5;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
                b[0x18]++;
            }
            break;
        case 1:
            if (DisplayAnimation_TestFinished(b + 0xd4)) {
                s16 *p = (s16 *)(b + 0x68);
                s32 x = *p++;
                s32 y = *p++;
                s32 z = *p;
                struct VecCB3F4 v;
                s16 *q = (s16 *)&v;
                s32 vx = x - 0x20;
                s32 vy = y + 0x180;
                s32 vz = z + 0x20;
                v.x = vx;
                q[1] = vy;
                q[2] = vz;
                TimedSpriteEffect_Create((u8 *)&v, 0xa0, 0x20, 1, 6, 0, 0, 0, 0);
                b[0x18]++;
            }
            break;
        case 2:
            b[7] = 1;
            break;
    }
}
