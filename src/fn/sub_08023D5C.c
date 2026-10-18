#include "global.h"
extern const u32 gUnk_08604E44[];
void Callback_InvokeActor(void *, u32);
void SceneActorState_DispatchPhaseHandler(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604E44[p[0xae]]);
}
