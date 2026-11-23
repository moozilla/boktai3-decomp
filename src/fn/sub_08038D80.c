#include "wireless_trade_menu.h"
#include "global.h"
#include "render.h"
#include "engine_pause_flags.h"
int WirelessExchange_ResolveOutcome(struct WirelessGameExchange *);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void WirelessGameMenu_UpdateOutcomeExchange(struct WirelessGameExchange *p)
{
    u32 data;
    if (p->entered) {
        p->entered = 0;
        p->outcome = WirelessExchange_ResolveOutcome(p);
        data = p->outcome;
        WirelessExchange_SendPacket(p, 5, &data, 1);
        Engine_SetPauseFlags(4);
    }
    if (test(&p->exchangeFlags, 0x300) == 0x300) {
        p->exchangeFlags &= ~WIRELESS_EXCHANGE_OUTCOME;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_RESULTS_DELAY);
    } else
        p->timer++;
}
