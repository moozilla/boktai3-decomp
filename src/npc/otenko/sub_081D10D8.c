#include "npc/otenko/otenko_actor.h"
#include "engine/ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetKeywordValue(s32, s32);
struct OtenkoActor *Actor_FindById(void *, s32);
void Otenko_FollowSceneTarget(void);
void Otenko_SetBehaviorTwoFromScript(void)
{
    struct OtenkoActor *p;
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
    {
        u8 *q = &p->controlled;
        s32 zero = 0, one = 1;
        void (*f)(void);
        *q = one;
        {
            s32 kind = 0x12;
            p->stage = zero;
            p->stageEntered = zero;
            p->state = kind;
        }
        p->timer = zero;
        p->commandEntered = one;
        f = Otenko_FollowSceneTarget;
        p->prevState = 0x10;
        p->stateCallback = f;
        p->stateFrame = zero;
        p->animationEntered = one;
        p->animationPhase = zero;
    }
}
