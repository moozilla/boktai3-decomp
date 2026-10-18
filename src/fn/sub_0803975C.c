#include "global.h"

struct S {
    u8 f[0x32];
    u8 a;
};
struct S *WirelessExchange_Get(void);
u32 WirelessGame_IsItemToggleEnabled(void)
{
    struct S *p = WirelessExchange_Get();
    if (!p)
        return 0;
    return p->a;
}
