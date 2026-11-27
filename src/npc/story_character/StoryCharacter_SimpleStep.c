#include "global.h"
#include "npc/story_character/state_actor.h"
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StoryCharacter_SimpleStep(void *a, struct StateActor *p)
{
    u8 hit;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->displayFlags = m | p->displayFlags;
    }
    hit = 0;
    if ((u8)Callback_InvokeActor(p, p->animationCallback) != 1 && sub_081C8FB0(a, p, 0)) {
        hit = 1;
        {
            u32 m = 0x800;
            p->displayFlags = m | p->displayFlags;
        }
    }
}
