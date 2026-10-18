#include "global.h"
extern const u32 gUnk_0860503C[];
void Callback_InvokeActor(void *, u32);
void GridAttack_UpdateSpecialActions(u8 *p)
{
    Callback_InvokeActor(p, gUnk_0860503C[p[0xAD]]);
}
