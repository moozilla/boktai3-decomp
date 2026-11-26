#include "global.h"
struct Panel {
    u8 bytes[0x25c];
};
struct P {
    u8 pad[0x19];
    u8 state;
    u8 pad1[0x12];
    struct Panel panels[2];
};
int Wireless_GetStatus(void);
void WirelessPeerMenu_UpdateConnectionState(struct P *);
int Callback_InvokeActor(void *, const void *);
void WirelessPeerPanel_Update(struct P *, struct Panel *, int);
extern const void *gUnk_08605780[];
int sub_08037B20(struct P *p)
{
    struct Panel *q;
    int i;
    if (!Wireless_GetStatus()) {
        WirelessPeerMenu_UpdateConnectionState(p);
        Callback_InvokeActor(p, gUnk_08605780[p->state]);
        q = p->panels;
        i = 0;
        do {
            WirelessPeerPanel_Update(p, q, i);
            i++;
            q++;
        } while (i <= 1);
    }
    return 0;
}
