#include "global.h"
extern const u32 gUnk_08604EF8[];
void Callback_InvokeActor(void *, u32);
void SceneActorState_UpdatePassiveActions(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604EF8[p[0xAD]]);
}
