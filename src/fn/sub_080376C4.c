#include "global.h"
#include "input.h"
struct PeerMenu {
    u8 pad0[0x19];
    u8 state, entered, pad1b[5];
    u32 timer;
};
int Wireless_IsSessionActive(void);
void Wireless_RequestAdvanceConnection(void);
void WirelessPeerMenu_UpdateCancelInput(struct PeerMenu *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 15) {
        int busy = Wireless_IsSessionActive();
        if (busy) {
            if (gPadState[1] & 2) {
                Wireless_RequestAdvanceConnection();
                {
                    u32 state = 7;
                    u32 zero = 0;
                    p->state = state;
                    p->entered = 1;
                    p->timer = zero;
                }
            }
        } else {
            if (gPadState[1] & 0x200) {
                Wireless_RequestAdvanceConnection();
                p->state = 7;
                p->entered = 1;
                p->timer = busy;
            }
        }
    }
    p->timer++;
}
