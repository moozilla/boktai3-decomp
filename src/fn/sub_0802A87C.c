#include "global.h"
void GridAttackActor_SetAnimation(u8 *, u32, u32, u32);
void Callback_InvokeActor(u8 *, void (*)(u8 *));
void GridAttack_UpdateAlternateIdleState(u8 *s)
{
    if (s[0xB1]) {
        s[0xB1] = 0;
        GridAttackActor_SetAnimation(s, 2, 1, 0);
    }
    {
        u32 zero;
        u32 *p = (u32 *)(s + 0x178);
        zero = 0;
        *p = zero;
        s[0x17C] = zero;
    }
    {
        u32 mask = 12;
        u32 *flags = (u32 *)(s + 0x9C);
        *flags |= mask;
    }
    Callback_InvokeActor(s, *(void (**)(u8 *))(s + 0x108));
}
