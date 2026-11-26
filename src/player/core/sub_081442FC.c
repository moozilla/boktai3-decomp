#include "global.h"
void Callback_InvokeActor(void *, u32);
void Player_InvokeStoredCallback(u8 *p)
{
    Callback_InvokeActor(p, *(u32 *)(p + 0xc88));
}
