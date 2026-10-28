#include "global.h"
#include "input.h"
struct P {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2[7];
    u8 countdown, retries, ready;
    u8 pad3[14];
    u8 cancel;
    u8 pad4[0xa7];
    u8 wait;
    u8 pad5[5];
    u32 requested;
};
void WirelessGameMenu_InitItemChoices(struct P *);
void WirelessGameMenu_InitEquipmentChoices(struct P *);
void SceneHud_EnterOptionControls(void);
void StatusPanel_BeginRefresh(void);
void WirelessGameMenu_SetStatusPanelState(struct P *, int);
void WirelessGameMenu_SignalStatusPanel(struct P *);
void Sound_PlaySong(int);
static inline u32 test(u16 *flags, u32 mask)
{
    return *flags & mask;
}
void WirelessGameMenu_UpdateSelection(struct P *p)
{
    if (p->entered) {
        u32 zero = 0;
        p->entered = zero;
        WirelessGameMenu_InitItemChoices(p);
        WirelessGameMenu_InitEquipmentChoices(p);
        p->requested = zero;
        SceneHud_EnterOptionControls();
        p->ready = 1;
        p->countdown = 8;
        p->wait = 6;
        WirelessGameMenu_SetStatusPanelState(p, 2);
    }
    if (gPadState[1] & 8) {
        p->requested = 1;
        p->countdown = 8;
    }
    if (p->ready && p->timer > 16 && p->requested) {
        u32 value = p->countdown;
        short countdown = value; /* s16 copy steers register allocation */
        if (countdown == 0) {
            u32 four;
            u8 *state;
            Sound_PlaySong(0xdd);
            four = 4;
            state = &p->state;
            *state = four;
            p->entered = 1;
            p->timer = countdown;
            StatusPanel_BeginRefresh();
            WirelessGameMenu_SignalStatusPanel(p);
            if (p->cancel && p->retries)
                p->retries--;
            return;
        }
        p->countdown = value - 1;
    }
    p->timer++;
}
