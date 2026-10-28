#include "global.h"
#include "input.h"
struct PeerMenu {
    u8 pad0[0x19];
    u8 state, entered, pad1b[5];
    u32 timer;
    u8 pad24[0x4c8];
    int window;
};
void TextWindow_Refresh(int, int);
void Sound_PlaySong(int);
void Wireless_RequestAdvanceConnection(void);
void WirelessPeerMenu_UpdateDismissibleNotice(struct PeerMenu *p)
{
    if (p->entered) {
        p->entered = 0;
        TextWindow_Refresh(p->window, 2);
        Sound_PlaySong(402);
    }
    if (p->timer > 35) {
        if (gPadState[1] & 3) {
            Wireless_RequestAdvanceConnection();
            {
                u32 state = 7;
                u32 zero = 0;
                p->state = state;
                p->entered = 1;
                p->timer = zero;
            }
        }
    }
    p->timer++;
}
