#include "npc/manager/scene_actor_lifecycle.h"
void SceneCharacter_StartAnimationSeven(void *unused, struct SceneActor *p)
{
    if (p->animationEntered) {
        int r;
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 15:
                r = 7;
                break;
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
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->animationPhase = 0;
    }
}
