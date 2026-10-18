#include "global.h"
extern const u32 gUnk_08604EE0[];
void Callback_InvokeActor(void *, u32);
void SceneActorState_DispatchSecondaryAction(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604EE0[p[0xae]]);
}
