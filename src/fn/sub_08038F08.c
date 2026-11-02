#include "global.h"
#include "runtime.h"
struct Menu {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2a[11];
    u8 retries, result;
    u8 pad37[0x99];
    int window;
    u8 padd4[8];
    int delta;
};
struct Save {
    u8 pad0[12];
    int score;
};
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
void WirelessTradeMenu_UpdateSaveResult(struct Menu *p)
{
    int *window = &p->window;
    if (p->entered) {
        s32 score;
        p->entered = 0;
        score = ((struct Save *)gSessionState)->score;
        score += p->delta;
        ((struct Save *)gSessionState)->score = WirelessTradeMenu_ClampScore(score);
    }
    if (p->timer == 60) {
        p->result = Save_WriteHeader();
        if (p->result == 1) {
            TextWindow_Refresh(*window, 14);
            Sound_PlaySong(686);
        } else {
            TextWindow_Refresh(*window, 15);
            Sound_PlaySong(402);
        }
    }
    if (p->timer == 240) {
        if (p->result == 1 || ++p->retries > 2) {
            {
                int state = 14;
                u8 *dest = &p->state;
                int zero = 0;
                *dest = state;
                p->entered = 1;
                p->timer = zero;
            }
        } else {
            TextWindow_Refresh(*window, 11);
            {
                int state = 13;
                u8 *dest = &p->state;
                *dest = state;
                p->entered = 1;
                p->timer = 0;
            }
        }
    } else
        p->timer++;
}
