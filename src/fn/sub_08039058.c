#include "global.h"
struct Menu {
    u8 pad0[0x28];
    u8 state, entered, pending;
    u8 pad2b[0x29];
    u8 message[16];
};
int Wireless_GetStatus(void);
void Callback_InvokeActor(void *, void *);
int Wireless_QueueNiPacket(void *);
extern void *gUnk_086057A0[];
int WirelessTradeMenu_Update(struct Menu *p)
{
    int busy = Wireless_GetStatus();
    if (!busy) {
        Callback_InvokeActor(p, gUnk_086057A0[p->state]);
        if (p->pending) {
            if (Wireless_QueueNiPacket(p->message) >= 0)
                p->pending = busy;
        }
    }
    return 0;
}
