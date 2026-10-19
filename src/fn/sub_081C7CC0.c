#include "global.h"
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 g[0xd4 - 0xa];
    u8 d4[1];
};
#define B(o) (*(u8 *)((u8 *)p + (o)))
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
void StateActor_UpdateAnimation18State(u32 a, struct S *p)
{
    u32 r;
    s32 state;

    if (p->b9 != 0) {
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
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 0x12;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        B(0x18) = 0;
    }

    state = B(0x18);
    switch (state) {
        case 0:
            if (DisplayAnimation_TestFinished(p->d4) == 0)
                break;
            B(0x18)++;
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
                    r = 0;
                    break;
                case 27:
                    r = 0x13;
                    break;
                case 28:
                case 29:
                default:
                    r = 0;
                    break;
            }
            Actor_PlayAnimationEntry(p, r, 2, 0, 4);
            break;
        case 1:
            if (DisplayAnimation_TestFinished(p->d4) == 0)
                break;
            p->b7 = state;
            break;
    }
}
