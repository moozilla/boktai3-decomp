#include "npc/otenko/otenko_actor.h"
#include "engine/ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetKeywordValue(s32, s32);
struct OtenkoActor *Actor_FindById(void *, s32);
void Actor_OnConfiguredAnimationUpdate(void);
void Otenko_SetBehaviorFourFromScript(void)
{
    struct OtenkoActor *p;
    void (*callback)(void);
    s32 state;
    if (!gSceneNpc)
        return;
    if (Script_SeekToKeyword(0x6e)) {
        void *manager = gSceneActorListManager;
        if (!manager)
            return;
        p = Actor_FindById(manager, Script_GetKeywordValue(0x6e, 0));
        if (!p)
            return;
    } else
        p = gSceneNpc;
    p->controlled = 1;
    state = 6;
    p->stage = 0;
    p->stageEntered = 0;
    p->state = state;
    p->timer = 0;
    p->commandEntered = 1;
    callback = Actor_OnConfiguredAnimationUpdate;
    p->prevState = 7;
    p->stateCallback = callback;
    p->stateFrame = 0;
    p->animationEntered = 1;
    p->animationPhase = 0;
}
