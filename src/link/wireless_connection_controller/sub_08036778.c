#include "engine/shared/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/shared/ram.h"
void sub_08223BC0(int);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_086056EC[];
int Wireless_ExchangeChildNiPacket(WirelessController *);
void Wireless_PollChildNiSend(WirelessController *);
int Wireless_UpdateChildNiSession(WirelessController *p)
{
    p->inputReady = 0;
    sub_08223BC0(0);
    rfu_getMasterSlave();
    Wireless_ExchangeChildNiPacket(p);
    Wireless_PollChildNiSend(p);
    {
        void **handlers = gUnk_086056EC;
        int op = p->operation;
        Callback_InvokeActor(p, handlers[op]);
    }
}
