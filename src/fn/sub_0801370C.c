#include "global.h"

void Callback_InvokeActor(u8 *p);
void ScaledShadow_Update(void);

void ScaledShadow_Enable(u8 *p)
{
    *(u32 *)(p + 0xC) &= ~1;
    *(u32 *)(p + 0x60) = (u32)ScaledShadow_Update;
    Callback_InvokeActor(p);
}
