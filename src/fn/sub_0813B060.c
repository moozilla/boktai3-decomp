#include "global.h"
void Callback_InvokeActor(void *, u32);
void Player_RunPendingCallback(u8 *p)
{
    u32 *q = (u32 *)(p + 0xca8);
    if (*q) {
        Callback_InvokeActor(p, *q);
        *q = 0;
    }
}
