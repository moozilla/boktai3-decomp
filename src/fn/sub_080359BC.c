#include "global.h"
#include "ram.h"

void rfu_REQ_changeMasterSlave(void);
void rfu_waitREQComplete(void);
void rfu_REQ_recvData(void);
void Wireless_OnChildUniClockEvent(u16 v)
{
    if (gWirelessController != 0) {
        if (v == 0x27) {
            rfu_REQ_changeMasterSlave();
            rfu_waitREQComplete();
        } else
            rfu_REQ_recvData();
    }
}
