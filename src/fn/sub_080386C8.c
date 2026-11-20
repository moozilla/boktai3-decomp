#include "wireless_trade_menu.h"
#include "global.h"
#include "render.h"
void PaletteLevels_Reset(void);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
void WirelessGameItems_StartOutgoingItems(void);
void Hud_ClearTimer(void);
static inline void set(u32 mask)
{
    gEnginePauseFlags |= mask;
}
void WirelessGameMenu_UpdateRoundHandshake(struct WirelessGameExchange *p)
{
    u32 data;
    u32 bits;
    u32 flags;
    if (p->entered) {
        p->entered = 0;
        PaletteLevels_Reset();
        data = 0;
        WirelessExchange_SendPacket(p, 1, &data, 1);
        set(4);
        WirelessGameMenu_SetStatusPanelState(p, 1);
        p->local.itemMode = 0;
        WirelessGameItems_StartOutgoingItems();
    }
    bits = 128;
    flags = p->exchangeFlags;
    bits &= flags;
    if (bits) {
        Hud_ClearTimer();
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ROUND_RESULT_EXCHANGE);
    } else {
        if ((flags & WIRELESS_EXCHANGE_HANDSHAKE) != WIRELESS_EXCHANGE_HANDSHAKE)
            goto tick;
        p->exchangeFlags = flags & ~WIRELESS_EXCHANGE_HANDSHAKE;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_SELECTION);
    }
    WirelessGameMenu_SignalStatusPanel(p);
    return;
tick:
    p->timer++;
}
