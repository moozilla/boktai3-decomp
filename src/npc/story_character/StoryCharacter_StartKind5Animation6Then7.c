#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"

void StoryCharacter_StartKind5Animation6Then7(u32 ctx, struct SceneActor *p)
{
    u32 r;
    u32 phase;

    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 6;
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
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
        p->animationPhase = 0;
    }

    phase = p->animationPhase;
    switch (phase) {
        case 0:
            if (!DisplayAnimation_TestFinished(&p->drawable.common))
                break;
            switch (p->variant) {
                case 4:
                    r = 0;
                    break;
                case 5:
                    r = 7;
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
            Actor_PlayAnimationEntry(p, r, 1, 1, 4);
            p->animationPhase++;
            break;
        case 1:
            p->finished = phase;
            break;
        default:
            break;
    }
}
