#include "global.h"

struct S {
    u8 f[0x1e];
    u16 a;
};
struct S *WirelessExchange_Get(void);
u32 WirelessGame_GetRoundCount(void)
{
    struct S *p = WirelessExchange_Get();
    if (!p)
        return 0;
    return p->a;
}
