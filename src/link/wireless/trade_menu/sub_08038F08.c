#include "link/wireless/wireless_trade_menu.h"
#include "global.h"
#include "link/session/session_save.h"
int Save_WriteHeader(void);
void TextWindow_Refresh(int, int);
void Sound_PlaySong(int);
static inline s32 WirelessTradeMenu_ClampScore(s32 score)
{
    if (score > 9999)
        score = 9999;
    else if (score < 0)
        score = 0;
    return score;
}
void WirelessTradeMenu_UpdateSaveResult(struct WirelessGameExchange *p)
{
    int *window = &p->window;
    if (p->entered) {
        s32 score;
        p->entered = 0;
        score = ((struct SessionSave *)gSessionState)->points;
        score += p->total;
        ((struct SessionSave *)gSessionState)->points = WirelessTradeMenu_ClampScore(score);
    }
    if (p->timer == 60) {
        p->saveResult = Save_WriteHeader();
        if (p->saveResult == 1) {
            TextWindow_Refresh(*window, 14);
            Sound_PlaySong(686);
        } else {
            TextWindow_Refresh(*window, 15);
            Sound_PlaySong(402);
        }
    }
    if (p->timer == 240) {
        if (p->saveResult == 1 || ++p->saveRetries > 2) {
            WirelessGameMenu_EnterState(p, WIRELESS_MENU_EXIT);
        } else {
            TextWindow_Refresh(*window, 11);
            WirelessGameMenu_EnterState(p, WIRELESS_MENU_SAVE_RESULT);
        }
    } else
        p->timer++;
}
