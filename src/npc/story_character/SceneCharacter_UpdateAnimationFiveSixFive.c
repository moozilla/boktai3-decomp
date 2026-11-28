#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void SceneCharacter_UpdateAnimationFiveSixFive(void *unused, struct SceneActor *p)
{
    if (p->animationEntered) {
        s32 r;
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
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
                r = 5;
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
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->animationPhase = 0;
    }
    if (DisplayAnimation_TestFinished(&p->drawable.common)) {
        switch (p->animationPhase) {
            case 0: {
                s32 r;
                switch (p->variant) {
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
                        r = 6;
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
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->animationPhase++;
                break;
            }
            case 1: {
                s32 r;
                switch (p->variant) {
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
                        r = 7;
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
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->animationPhase++;
                break;
            }
            case 2:
                p->finished = 1;
                p->animationPhase++;
                break;
            default:
                return;
        }
    }
}
