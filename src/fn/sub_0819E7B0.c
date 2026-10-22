#include "global.h"
void Hresvelgr_SetAnimation(u8 *, void *, void *, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);

void sub_0819E7B0(u8 *p)
{
    if (p[0xb1] != 0) {
        s32 zero = 0;
        p[0xb1] = zero;
        Hresvelgr_SetAnimation(p, p + 0x194, p + 0x174, zero, 1, zero);
    }
    Callback_InvokeActor(p, *(void **)(p + 0x104));
}
