#include "global.h"
void Registry2388_ConfigureDisplay(void);
void Registry2388_UpdateIndicators(u8 *);
void Callback_InvokeActor(u8 *, u32);
void Registry2388_OnUpdate(u8 *p)
{
    Registry2388_ConfigureDisplay();
    Registry2388_UpdateIndicators(p);
    if (*(u32 *)(p + 0x55C))
        Callback_InvokeActor(p, *(u32 *)(p + 0x55C));
}
