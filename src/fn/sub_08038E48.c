#include "global.h"
#include "runtime.h"
struct P {
    u8 pad[0x1e];
    u16 round;
    u8 pad2[4];
    u32 timer;
    u8 state, entered;
    u8 pad3[5];
    u8 mode;
    u8 pad4[4];
    u8 done, flag;
    u8 pad5[14];
    u8 *left;
    u8 pad6[8];
    u8 *right;
    u8 pad7[0x78];
    void *script;
    int window;
    u8 pad8[8];
    u32 reward;
};
struct Global {
    u8 pad[12];
    u32 money;
};
int Wireless_IsSessionActive(void);
void WirelessGameMenu_OnResultPanelClosed(void);
void sub_0803B700(int, int, void *, void *, u32, u32, void *);
void WirelessGameMenu_LoadBackground(struct P *, int);
int TextWindow_Create(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);
void WirelessTradeMenu_UpdateResultsPanel(struct P *p)
{
    if (p->entered) {
        int offset;
        int side;
        u32 money;
        u8 *left;
        u8 *right;
        p->entered = 0;
        p->done = 0;
        offset = (p->round - 1) * 720;
        left = (u8 *)(offset + (u32)p->left);
        right = (u8 *)(offset + (u32)p->right);
        side = Wireless_IsSessionActive();
        money = ((struct Global *)gSessionState)->money;
        sub_0803B700(side, p->mode, left, right, p->reward, money,
                     WirelessGameMenu_OnResultPanelClosed);
    }
    if (p->done) {
        int *window = &p->window;
        u32 thirteen = 13;
        u8 *state = &p->state;
        u32 zero = 0;
        int w;
        *state = thirteen;
        p->entered = 1;
        p->timer = zero;
        p->flag = zero;
        WirelessGameMenu_LoadBackground(p, 3);
        w = TextWindow_Create(1, 6, 28, 6);
        *window = w;
        if (w >= 0) {
            TextWindow_SetScript(w, p->script);
            TextWindow_Refresh(*window, 11);
            TextWindow_StartPlayback(*window);
        }
    }
}
