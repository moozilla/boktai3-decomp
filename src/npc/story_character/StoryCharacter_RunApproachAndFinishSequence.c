#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void StoryCharacter_RunApproachAndFinishSequence(u32 a, struct SceneActor *p, s32 a2)
{
    if (p->animationEntered != 0) {
        s32 animId;
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
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
        p->animationPhase = 0;
        p->stateFrame = 0;
    }
    switch (p->animationPhase) {
        case 0:
            if (DisplayAnimation_TestFinished(&p->drawable.common) != 0) {
                s32 animId;
                switch (p->variant) {
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
                p->animationPhase++;
                p->stateFrame = 0;
            }
            break;
        case 1:
            if (a2 > 0xa) {
                s32 animId;
                switch (p->variant) {
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
                p->animationPhase++;
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(&p->drawable.common) != 0) {
                p->finished = 1;
            }
            break;
    }
}
