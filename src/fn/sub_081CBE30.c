#include "global.h"
struct Vec081CBE30 {
    s16 x, y, z, pad;
};
void Actor_PlayAnimationEntry(u8 *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
void TimedSpriteEffect_Create(struct Vec081CBE30 *, s32, s32, s32, s32, s32, s32, s32, s32);
/* Three-phase attack: play the wind-up, spawn an effect above the actor on
 * frame 15, then the follow-through after 0x56 frames. */
void StateActor_UpdateDelayedAttackEffect(u8 *a, u8 *b, s32 inputFrame)
{
    s32 frame = inputFrame;
    u32 anim;
    s32 state;
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
                anim = 0;
                break;
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
            case 27:
            case 28:
                anim = 0;
                break;
            case 29:
                anim = 8;
                break;

            default:
                anim = 0;
                break;
        }

        Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
        {
            u32 zero = 0;
            b[0x18] = zero;
            *((u32 *)(b + 0x10)) = zero;
        }
    }
    state = b[0x18];
    switch (state) {
        case 0:
            if (frame == 15) {
                s16 *p = (s16 *)(b + 0x68);
                s32 x = *p++;
                s32 y = *p++;
                s32 z = *p;
                struct Vec081CBE30 v;
                s16 *q = (s16 *)&v;
                s32 vx = x - 0x20;
                s32 vy = y + 0x180;
                s32 vz = z + 0x20;
                v.x = vx;
                q[1] = vy;
                q[2] = vz;
                TimedSpriteEffect_Create(&v, 0xA0, 0x20, 1, 1, state, state, state, state);
            }
            if (DisplayAnimation_TestFinished(b + 0xd4))
                b[0x18] = 1;
            break;
        case 1:
            if ((*((u32 *)(b + 0x10))) > 0x56) {
                b[0x18] = 2;
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
                        anim = 0;
                        break;
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
                    case 27:
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 9;
                        break;

                    default:
                        anim = 0;
                        break;
                }

                Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(b + 0xd4))
                b[7] = 1;
            break;
    }
}

