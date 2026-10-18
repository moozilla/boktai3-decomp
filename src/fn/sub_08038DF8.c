#include "global.h"
struct P {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2[0xae];
    u32 title;
};
void Wireless_RequestResetConnection(void);
void WirelessGameMenu_SignalStatusPanel(struct P *);
void Script_Call(u32, int);
void WirelessGameMenu_UpdateResultsDelay(struct P *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 119) {
        u32 eleven;
        u8 *state;
        u32 zero;
        Wireless_RequestResetConnection();
        eleven = 11;
        state = &p->state;
        zero = 0;
        *state = eleven;
        p->entered = 1;
        p->timer = zero;
        WirelessGameMenu_SignalStatusPanel(p);
        if (p->title)
            Script_Call(p->title, 0);
    } else
        p->timer++;
}
