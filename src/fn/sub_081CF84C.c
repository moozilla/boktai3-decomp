#define SceneNpc_PlayAnimation12 sub_081CF84C
#include "global.h"
#include "state_actor.h"

s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
void sub_081CF84C(void *unused, struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    if (p->enter) {
        s32 zero = 0;
        p->enter = zero;
        p->done = zero;
        Actor_PlayAnimationEntry(p, 12, 0, 0, 4);
        tag->state.halves.fieldA = zero;
    }
    {
        u32 mask = 0x409;
        p->displayFlags |= mask;
    }
    if ((u8)p->animationCallback(p) != 1)
        p->done = 1;
}
