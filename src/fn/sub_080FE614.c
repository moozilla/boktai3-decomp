#include "scene_actor_state.h"

s32 Axe_PrepareUpdate(void *p);
s32 Axe_UpdateEffects(void *s);
s32 Callback_InvokeActor(void *, void *);
void SceneActorControl_DispatchControl(void *);

s32 Axe_OnUpdate(struct ChaseActor *p)
{
    u8 idx;
    u32 *tbl;

    Axe_PrepareUpdate(p);
    idx = p->base.state2A8;
    tbl = (u32 *)p->base.stateCallbacks;
    Callback_InvokeActor(p, tbl[idx]);
    SceneActorControl_DispatchControl(p);
    Axe_UpdateEffects(p);
    return 1;
}
