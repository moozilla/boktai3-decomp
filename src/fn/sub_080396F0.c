#include "global.h"

struct S {
    u8 f[0x2e];
    u8 a;
};
struct S *WirelessExchange_Get(void);
void WirelessGame_NotifyOpponentDefeated(void)
{
    struct S *p = WirelessExchange_Get();
    if (p)
        p->a = 1;
}
