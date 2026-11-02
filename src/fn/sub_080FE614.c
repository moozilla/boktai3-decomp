#include "scene_actor_state.h"

void Axe_PrepareUpdate(void *);
void Axe_UpdateEffects(void *);
void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);

s32 Axe_OnUpdate(struct SceneActorRegistryResetView *p)
{
    u8 idx;
    u32 *tbl;

    Axe_PrepareUpdate(p);
    idx = p->state2A8;
    tbl = (u32 *)p->callbacks27C;
    Callback_InvokeActor(p, tbl[idx]);
    SceneActorControl_DispatchControl(p);
    Axe_UpdateEffects(p);
    return 1;
}
