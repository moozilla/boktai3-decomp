#include "global.h"
struct Panel {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
};
struct Panel *WirelessExchange_Get(void);
void WirelessGame_BeginResults(void)
{
    struct Panel *p = WirelessExchange_Get();
    if (p) {
        int state = 12;
        u8 *dest = &p->state;
        int zero = 0;
        *dest = state;
        p->entered = 1;
        p->timer = zero;
    }
}
