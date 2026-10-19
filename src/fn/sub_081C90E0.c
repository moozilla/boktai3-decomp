#include "global.h"
#include "state_actor.h"

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);

void StateActor_StartKind9Or10Animation4(u32 ctx, struct StateActor *p)
{
    u32 r;
    if (p->enter == 0)
        return;
    p->enter = 0;
    p->done = 0;
    if (p->kind == 9) {
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 4;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 0;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    } else {
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 4;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 0;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    p->done = 1;
}
