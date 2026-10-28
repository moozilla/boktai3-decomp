// MATCHING_CONCESSION: register-pinning
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
void WirelessTradeMenu_UpdateSaveResult(struct Menu *p)
{
    int *window = &p->window;
    if (p->entered) {
        struct Save **save;
        int score;
        int delta;
        p->entered = 0;
        save = &gSessionState;
        score = (*save)->score;
        delta = p->delta;
        score += delta;
        {
            register int maximum asm("r3") = 9999;
            if (score > maximum)
                score = maximum;
            else if (score < 0)
                score = 0;
            (*save)->score = score;
        }
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
