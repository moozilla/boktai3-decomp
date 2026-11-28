#include "global.h"
#include "npc/story_character/state_actor.h"

extern void (*const gUnk_08604F08[])(void *, int);

u32 SceneActors_TryApplyRequestedState(struct StateActor *p)
{
    if ((u8)(p->actionAnimation - 1) > 39)
        return 0;
    SceneActorState_EnterAction(p, gUnk_08604F08[p->actionAnimation], p->actionAnimation);
    return 1;
}
