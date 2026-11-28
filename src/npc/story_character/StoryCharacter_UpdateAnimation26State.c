#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void StoryCharacter_UpdateAnimation26State(u32 a, struct SceneActor *p)
{
    u32 entry;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
        case 27:
            entry = 0x1a;
            break;
        case 4 ... 26:
        case 28 ... 29:
        default:
            entry = 0;
            break;
        }
        Actor_PlayAnimationEntry(p, entry, 3, 1, 4);
    }
    if (DisplayAnimation_TestFinished(&p->drawable.common) != 0)
        p->finished = 1;
}
