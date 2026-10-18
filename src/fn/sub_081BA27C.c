#include "global.h"
void Registry2388_ConfigureDisplay(void);
void sub_081B98D8(u8 *);
void Callback_InvokeActor(u8 *, u32);
void Registry2388_OnUpdate(u8 *p)
{
    Registry2388_ConfigureDisplay();
    sub_081B98D8(p);
    if (*(u32 *)(p + 0x55C))
        Callback_InvokeActor(p, *(u32 *)(p + 0x55C));
}
