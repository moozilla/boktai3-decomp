#include "global.h"
struct Panel {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
};
struct Panel *WirelessExchange_Get(void);
void WirelessGame_InitDisplayAndLinkCallbacks(void *);
void WirelessGame_RestartSession(void)
{
    struct Panel *p = WirelessExchange_Get();
    u8 *alias2;
    struct Panel *alias;
    if (p) {
        int one = 1;
        u8 *state = alias2 = &p->state;
        int zero = 0;
        *state = one;
        state++;
        *state = one;
        p->timer = zero;
        alias = p;
        WirelessGame_InitDisplayAndLinkCallbacks(alias);
    }
}
