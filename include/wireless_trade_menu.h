#ifndef GUARD_WIRELESS_TRADE_MENU_H
#define GUARD_WIRELESS_TRADE_MENU_H

#include "runtime.h"

struct WirelessGameDisplay;

enum { WIRELESS_ROUND_FRAMES = 720 };

enum WirelessGameMenuState {
    WIRELESS_MENU_IDLE = 0,
    WIRELESS_MENU_STARTUP_DELAY = 1,
    WIRELESS_MENU_ROUND_HANDSHAKE = 2,
    WIRELESS_MENU_SELECTION = 3,
    WIRELESS_MENU_SELECTION_EXCHANGE = 4,
    WIRELESS_MENU_READY_EXCHANGE = 5,
    WIRELESS_MENU_ROUND_PREPARATION = 6,
    WIRELESS_MENU_ACTIVE_ROUND = 7,
    WIRELESS_MENU_ROUND_RESULT_EXCHANGE = 8,
    WIRELESS_MENU_OUTCOME_EXCHANGE = 9,
    WIRELESS_MENU_RESULTS_DELAY = 10,
    WIRELESS_MENU_RESULTS_IDLE = 11,
    WIRELESS_MENU_RESULTS_PANEL = 12,
    WIRELESS_MENU_SAVE_RESULT = 13,
    WIRELESS_MENU_EXIT = 14,
};

enum WirelessExchangeFlag {
    WIRELESS_EXCHANGE_HANDSHAKE = 0x003,
    WIRELESS_EXCHANGE_SELECTION = 0x00C,
    WIRELESS_EXCHANGE_READY = 0x030,
    WIRELESS_EXCHANGE_PEER_RESULT = 0x080,
    WIRELESS_EXCHANGE_ROUND_RESULT = 0x0C0,
    WIRELESS_EXCHANGE_OUTCOME = 0x300,
};

struct WirelessGamePeerStats {
    u8 counts[4];
    u8 playerHealthPercent, opponentHealthPercent;
    u8 itemMode, result;
    u32 elapsed;
};

/* Fields used by the trade menu, packet callbacks and wireless game display.
 * The meanings of the entry and request latches are tentative. */
struct WirelessGameExchange {
    struct RuntimeTask task;
    u32 unk18;
    u16 roundTimeLeft, roundCount;
    u32 exchangeFlags, timer;
    u8 state, entered, pendingPacket, unk2B;
    u8 readyToSend, playerDefeated, opponentDefeated, outcome;
    u8 pendingEvent, selectionCountdown, enabled, selectionReady;
    u8 resultPanelClosed, saveRetries, saveResult, roundActive;
    u8 queuedCounts[4];
    struct WirelessGamePeerStats local, remote;
    u8 sendPacket[12];
    struct WirelessGameDisplay *display;
    u32 itemCount, unk68, extraItemCount;
    u32 itemMask, equipmentMask, extraItemMask;
    u16 items[16], unk9C[16], extraItems[8];
    void *textScript;
    s32 window;
    u32 completionScript, resultsScript;
    s32 total;
    u16 initialWeight, weights[4];
    u8 choice, counter, equipmentMode;
    u8 unkED[3];
    u32 selectionRequested;
    u8 peerEntries[2][16];
};

struct WirelessGameExchange *WirelessExchange_Get(void);

static inline void WirelessGameMenu_EnterState(struct WirelessGameExchange *menu,
                                               enum WirelessGameMenuState state)
{
    menu->state = state;
    menu->entered = 1;
    menu->timer = 0;
}

#endif
