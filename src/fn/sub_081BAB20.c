#include "global.h"
void Registry3F74_ConfigureDisplay(void);
void sub_081BA6F8(u8 *);
void sub_0824923C(u8 *, u32);
void Registry3F74_OnUpdate(u8 *p)
{
    Registry3F74_ConfigureDisplay();
    sub_081BA6F8(p);
    if (*(u32 *)(p + 0x260))
        sub_0824923C(p, *(u32 *)(p + 0x260));
}
