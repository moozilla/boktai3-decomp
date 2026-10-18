#include "global.h"
void Registry2388_ConfigureDisplay(void);
void sub_081B98D8(u8 *);
void sub_0824923C(u8 *, u32);
void Registry2388_OnUpdate(u8 *p)
{
    Registry2388_ConfigureDisplay();
    sub_081B98D8(p);
    if (*(u32 *)(p + 0x55C))
        sub_0824923C(p, *(u32 *)(p + 0x55C));
}
