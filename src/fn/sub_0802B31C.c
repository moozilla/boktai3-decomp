#include "global.h"
extern const u32 gUnk_08604FF0[];
void Callback_InvokeActor(void *, u32);
void GridAttack_DispatchSpecialAnimation(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08604FF0[p[0xae]]);
}
