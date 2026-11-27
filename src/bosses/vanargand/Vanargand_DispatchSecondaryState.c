#include "global.h"
extern const u32 gUnk_08611F9C[];
void Callback_InvokeActor(void *, u32);
void Vanargand_DispatchSecondaryState(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611F9C[p[0xae]]);
}
