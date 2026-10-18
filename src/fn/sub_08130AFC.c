#include "global.h"
void Callback_InvokeActor(u8 *, u32);
void SceneActorState_DispatchTable2A0State(u8 *p)
{
    u32 i = p[0x2a9];
    Callback_InvokeActor(p, ((u32 *)*(u32 *)(p + 0x2a0))[i]);
}
