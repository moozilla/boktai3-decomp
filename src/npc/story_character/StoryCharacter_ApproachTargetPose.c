#include "npc/manager/scene_actor_lifecycle.h"
s32 ScalarApproach_StepSixteenth(s32, s32);
void StoryCharacter_ApproachTargetPose(u32 a, struct SceneActor *p)
{
    SceneActor_SetFlags(p, 0x400);
    if (p->animationEntered != 0) {
        u32 r1;
        p->animationEntered = 0;
        p->finished = 0;
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
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r1 = 0;
                break;
            case 27:
                r1 = 0x10;
                break;
            default:
                r1 = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r1, 1, 1, 4);
        p->animationPhase = 0;
    }
    switch (p->animationPhase) {
        case 0:
            p->body.position.x = ScalarApproach_StepSixteenth(p->body.position.x, p->target.x);
            p->body.position.y = ScalarApproach_StepSixteenth(p->body.position.y, p->target.y);
            p->body.position.z = ScalarApproach_StepSixteenth(p->body.position.z, p->target.z);
            if (*(u32 *)&p->body.position == *(u32 *)&p->target &&
                p->body.position.z == p->target.z) {
                p->animationPhase++;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
