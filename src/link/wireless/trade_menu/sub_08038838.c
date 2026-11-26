#include "link/wireless/wireless_trade_menu.h"
#include "global.h"
#include "engine/word_flags.h"

void WirelessGame_AddWeightedScore(int, int);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void WirelessGameMenu_ConsumeSelectedItem(struct WirelessGameExchange *);
void WirelessGameMenu_ConsumeSelectedEquipment(struct WirelessGameExchange *);
void WirelessExchange_ApplyReceivedCounts(struct WirelessGameExchange *);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);


void WirelessGameMenu_UpdateSelectionExchange(struct WirelessGameExchange *p)
{
    u32 data;
    if (p->entered) {
        struct WirelessGamePeerStats *peer;
        u32 packed;
        int total;
        int i;
        int shift;
        u8 *q;
        p->entered = 0;
        do {
            data = 0;
            peer = &p->local;
            packed = 0;
            total = 0;
            i = 0;
            shift = 0;
            q = peer->counts;
        } while (0);
        do {
            int value;
            u32 flag = peer->itemMode;
            if (flag) {
                value = *q;
            } else {
                int old = *q;
                u8 *score;
                int n;
                score = p->queuedCounts;
                score += i;
                n = *score;
                value = old + n;
                total += n;
                *score = (float)flag;
            }
            peer->counts[i] = 0;
            if (value > 15) {
                value = 15;
            }
            packed |= value << shift;
            q++;
            i++;
            shift += 4;
        } while (i <= 3);
        if (total > 0) {
            WirelessGame_AddWeightedScore(3, 1);
        }
        data |= packed;
        WirelessExchange_SendPacket(p, 2, &data, 1);
        WirelessGameMenu_SetStatusPanelState(p, 1);
        WirelessGameMenu_ConsumeSelectedItem(p);
        WirelessGameMenu_ConsumeSelectedEquipment(p);
    }
    if (WordFlags_Test(&p->exchangeFlags, WIRELESS_EXCHANGE_SELECTION) == WIRELESS_EXCHANGE_SELECTION) {
        WirelessExchange_ApplyReceivedCounts(p);
        {
            u32 mask = ~12;
            p->exchangeFlags &= mask;
        }
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_READY_EXCHANGE);
        WirelessGameMenu_SignalStatusPanel(p);
    } else {
        p->timer++;
    }
}
