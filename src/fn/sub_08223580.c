#include "global.h"
#include "rfu_manager.h"

void rfu_REQ_sendData(u32);

void RfuManager_SendData(u8 a)
{
    if (**(u8 **)0x03006A80 == 0) {
        u8 t = gRfuLinkManager.childClockSlave_flag;
        a = 0;
        if (t == 1)
            a = 1;
    } else {
        gRfuLinkManager.parentAck_flag = 0;
    }
    rfu_REQ_sendData(a);
}
