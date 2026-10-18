#include "global.h"
extern const u32 gUnk_08604EF4[];
void Callback_InvokeActor(void *, u32);
void SceneActorState_UpdateSecondaryActions(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604EF4[p[0xAD]]);
}
