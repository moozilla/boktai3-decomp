#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StoryCharacter_SimpleStep(void *context, struct SceneActor *p)
{
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    }
    SceneActor_SetFlags(p, 0x1000);
    if ((u8)Callback_InvokeActor(p, p->query) != 1 && sub_081C8FB0(context, p, 0)) {
        SceneActor_SetFlags(p, 0x800);
    }
}
