#include "npc/otenko/otenko_actor.h"
#include "engine/engine.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void Actor_OnHiddenIdleUpdate(void);
void Otenko_UpdateStopped(void *unused, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 5, 2, 0, 4);
        {
            s32 two = 2;
            if ((npc->commandFlags & two) == 0)
                Sound_PlaySong(0x11F);
        }
        {
            s32 m = -3;
            npc->commandFlags = m & npc->commandFlags;
        }
    }
    npc->displayFlags |= 1;
    if ((u8)Callback_InvokeActor(npc, npc->animationCallback) != 1 &&
        DisplayAnimation_TestFinished(&npc->display)) {
        void (*next)(void);
        npc->done = 1;
        next = Actor_OnHiddenIdleUpdate;
        npc->prevState = 1;
        npc->stateCallback = next;
        npc->stateFrame = 0;
        npc->animationEntered = 1;
        npc->animationPhase = 0;
    }
}
