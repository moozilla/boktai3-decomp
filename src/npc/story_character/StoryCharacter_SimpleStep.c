#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StoryCharacter_SimpleStep(void *a, struct SceneActor *p)
{
    u8 hit;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    }
    SceneActor_SetFlags(p, 0x1000);
    hit = 0;
    if ((u8)Callback_InvokeActor(p, p->query) != 1 && sub_081C8FB0(a, p, 0)) {
        hit = 1;
        SceneActor_SetFlags(p, 0x800);
    }
}
