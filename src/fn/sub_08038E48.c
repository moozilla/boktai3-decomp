#include "wireless_trade_menu.h"
#include "global.h"
#include "session_save.h"
int Wireless_IsSessionActive(void);
void WirelessGameMenu_OnResultPanelClosed(void);
int sub_0803B700(int, int, int, int, int, int, u32);
void WirelessGameMenu_LoadBackground(struct WirelessGameExchange *, int);
int TextWindow_Create(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);
void WirelessTradeMenu_UpdateResultsPanel(struct WirelessGameExchange *p)
{
    if (p->entered) {
        int offset;
        int side;
        s32 points, localTime, remoteTime;
        p->entered = 0;
        p->resultPanelClosed = 0;
        offset = (p->roundCount - 1) * 720;
        localTime = offset + p->local.elapsed;
        remoteTime = offset + p->remote.elapsed;
        side = Wireless_IsSessionActive();
        points = ((struct SessionSave *)gSessionState)->points;
        sub_0803B700(side, p->outcome, localTime, remoteTime, p->total, points,
                     (u32)WirelessGameMenu_OnResultPanelClosed);
    }
    if (p->resultPanelClosed) {
        int *window = &p->window;
        int w;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_SAVE_RESULT);
        p->saveRetries = 0;
        WirelessGameMenu_LoadBackground(p, 3);
        w = TextWindow_Create(1, 6, 28, 6);
        *window = w;
        if (w >= 0) {
            TextWindow_SetScript(w, p->textScript);
            TextWindow_Refresh(*window, 11);
            TextWindow_StartPlayback(*window);
        }
    }
}
