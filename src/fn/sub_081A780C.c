#include "global.h"
extern const u32 gUnk_08611F48[];
void Callback_InvokeActor(void *, u32);
void Vanargand_DispatchPrimaryState(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611F48[p[0xae]]);
}
