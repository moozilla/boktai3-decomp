#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void StoryCharacter_RunEntryAnimSequence(u32 a, struct SceneActor *p, s32 a2)
{
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        Actor_PlayAnimationEntry(p, 0, 2, 0, 4);
        p->animationPhase = 0;
        p->stateFrame = 0;
    }
    switch (p->animationPhase) {
        case 0: {
            u32 r1;
            if (p->stateFrame <= 10)
                break;
            switch (p->variant) {
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
                case 28:
                case 29:
                    r1 = 0;
                    break;
                case 27:
                    r1 = 0x15;
                    break;
                default:
                    r1 = 0;
                    break;
            }
            Actor_PlayAnimationEntry(p, r1, 2, 0, 4);
            p->animationPhase++;
            break;
        }
        case 1:
            if (DisplayAnimation_TestFinished(&p->drawable.common) != 0) {
                u32 zero = 0;
                Actor_PlayAnimationEntry(p, zero, 2, 0, 4);
                p->animationPhase++;
                p->stateFrame = zero;
            }
            break;
        case 2:
            if (a2 > 10) {
                p->finished = 1;
            }
            break;
    }
}
