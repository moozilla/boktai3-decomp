#include "global.h"
extern const u32 gUnk_08604E9C[];
void Callback_InvokeActor(void *, u32);
void SceneActorState_DispatchPrimaryAction(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604E9C[p[0xae]]);
}
