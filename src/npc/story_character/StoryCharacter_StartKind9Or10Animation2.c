#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"

void StoryCharacter_StartKind9Or10Animation2(u32 ctx, struct SceneActor *p)
{
    u32 r;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        if (p->variant == 9) {
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
            Actor_PlayAnimationEntry(p, r, 2, 0, 2);
        } else {
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
            Actor_PlayAnimationEntry(p, r, 2, 0, 2);
        }
        p->animationPhase = 0;
    }
    if (p->animationPhase != 0)
        return;
    if (!DisplayAnimation_TestFinished(&p->drawable.common))
        return;
    Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    p->finished = 1;
    p->animationPhase = 1;
}
