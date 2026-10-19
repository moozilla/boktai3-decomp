#include "global.h"
void Registry3F74_ConfigureDisplay(void);
void Registry3F74_UpdateAnimations(u8 *);
void Callback_InvokeActor(u8 *, u32);
void Registry3F74_OnUpdate(u8 *p)
{
    Registry3F74_ConfigureDisplay();
    Registry3F74_UpdateAnimations(p);
    if (*(u32 *)(p + 0x260))
        Callback_InvokeActor(p, *(u32 *)(p + 0x260));
}
