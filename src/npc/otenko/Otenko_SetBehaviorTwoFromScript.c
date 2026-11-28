#include "npc/otenko/otenko_actor.h"
#include "engine/ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetKeywordValue(s32, s32);
struct OtenkoActor *Actor_FindById(void *, s32);
void Otenko_FollowSceneTarget(void);
void Otenko_SetBehaviorTwoFromScript(void)
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
    state = 0x12;
    p->stage = 0;
    p->stageEntered = 0;
    p->state = state;
    p->timer = 0;
    p->commandEntered = 1;
    callback = Otenko_FollowSceneTarget;
    p->prevState = 0x10;
    p->stateCallback = callback;
    p->stateFrame = 0;
    p->animationEntered = 1;
    p->animationPhase = 0;
}
