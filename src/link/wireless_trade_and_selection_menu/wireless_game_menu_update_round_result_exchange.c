#include "link/wireless_trade_and_selection_menu/wireless_trade_menu.h"
#include "engine/shared/global.h"
#include "engine/shared/word_flags.h"
#include "player/player.h"
void PaletteLevels_Reset(void);
void WirelessExchange_CaptureLocalResult(struct WirelessGameExchange *);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void Sound_FadeOutChannel10(int);
void PlayerCue_Stand(struct Player *, s32);
void WirelessGameMenu_UpdateRoundResultExchange(struct WirelessGameExchange *p)
{
    u32 data;
    if (p->entered) {
        p->entered = 0;
        PaletteLevels_Reset();
        WirelessExchange_CaptureLocalResult(p);
        {
            struct WirelessGamePeerStats *e = &p->local;
            data = (u16)e->elapsed | ((e->result & 3) << 16);
        }
        WirelessExchange_SendPacket(p, 4, &data, 1);
        WirelessGameMenu_SetStatusPanelState(p, 5);
        Sound_FadeOutChannel10(4);
        {
            u32 mask = 4;
            if (!(gPlayerSlotStatus[0] & mask))
                PlayerCue_Stand(gPlayers[0], -1);
        }
    }
    if (p->timer > 29 && WordFlags_Test(&p->exchangeFlags, WIRELESS_EXCHANGE_ROUND_RESULT) == WIRELESS_EXCHANGE_ROUND_RESULT) {
        p->exchangeFlags &= ~WIRELESS_EXCHANGE_ROUND_RESULT;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_OUTCOME_EXCHANGE);
    } else
        p->timer++;
}
