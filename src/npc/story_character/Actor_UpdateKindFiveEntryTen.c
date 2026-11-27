#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void Actor_UpdateKindFiveEntryTen(u32 a, struct SceneActor *p)
{
    u32 r;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 4:
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
                r = 0;
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
            case 29:
                r = 0;
                break;
            case 5:
                r = 0xa;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(&p->drawable.common) != 0)
        p->finished = 1;
}
