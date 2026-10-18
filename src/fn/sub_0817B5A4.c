#include "global.h"
extern const u32 gUnk_08611C5C[];
void Callback_InvokeActor(void *, u32);
void SceneActors_DispatchSubtypeHandler(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611C5C[p[0xae]]);
}
