#include "global.h"
#include "state_actor.h"

#define StateActor_StartKind5Animation6Then7 sub_081CA0A0

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);

void sub_081CA0A0(u32 ctx, struct StateActor *p)
{
    u32 r;
    u32 phase;

    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4: r = 0; break;
            case 5: r = 6; break;
            case 6: r = 0; break;
            case 7: r = 0; break;
            case 8: r = 0; break;
            case 9: r = 0; break;
            case 10: r = 0; break;
            case 11: r = 0; break;
            case 12: r = 0; break;
            case 13: r = 0; break;
            case 14: r = 0; break;
            case 15: r = 0; break;
            case 16: r = 0; break;
            case 17: r = 0; break;
            case 18: r = 0; break;
            case 19: r = 0; break;
            case 20: r = 0; break;
            case 21: r = 0; break;
            case 22: r = 0; break;
            case 23: r = 0; break;
            case 24: r = 0; break;
            case 25: r = 0; break;
            case 26: r = 0; break;
            case 27: r = 0; break;
            case 28: r = 0; break;
            case 29: r = 0; break;
            default: r = 0; break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
        p->phase = 0;
    }

    phase = p->phase;
    switch (phase) {
        case 0:
            if (!DisplayAnimation_TestFinished((u8 *)p + 0xd4))
                break;
            switch (p->kind) {
                case 4: r = 0; break;
                case 5: r = 7; break;
                case 6: r = 0; break;
                case 7: r = 0; break;
                case 8: r = 0; break;
                case 9: r = 0; break;
                case 10: r = 0; break;
                case 11: r = 0; break;
                case 12: r = 0; break;
                case 13: r = 0; break;
                case 14: r = 0; break;
                case 15: r = 0; break;
                case 16: r = 0; break;
                case 17: r = 0; break;
                case 18: r = 0; break;
                case 19: r = 0; break;
                case 20: r = 0; break;
                case 21: r = 0; break;
                case 22: r = 0; break;
                case 23: r = 0; break;
                case 24: r = 0; break;
                case 25: r = 0; break;
                case 26: r = 0; break;
                case 27: r = 0; break;
                case 28: r = 0; break;
                case 29: r = 0; break;
                default: r = 0; break;
            }
            Actor_PlayAnimationEntry(p, r, 1, 1, 4);
            p->phase++;
            break;
        case 1:
            p->done = phase;
            break;
        default:
            break;
    }
}
