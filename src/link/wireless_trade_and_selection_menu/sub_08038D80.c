#include "link/wireless_trade_and_selection_menu/wireless_trade_menu.h"
#include "engine/shared/global.h"
#include "engine/shared/render.h"
#include "engine/shared/engine_pause_flags.h"
#include "engine/shared/word_flags.h"
int WirelessExchange_ResolveOutcome(struct WirelessGameExchange *);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
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
    if (WordFlags_Test(&p->exchangeFlags, WIRELESS_EXCHANGE_OUTCOME) == WIRELESS_EXCHANGE_OUTCOME) {
        p->exchangeFlags &= ~WIRELESS_EXCHANGE_OUTCOME;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_RESULTS_DELAY);
    } else
        p->timer++;
}
