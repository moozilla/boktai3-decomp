#define gSerialStatus gSerialTransferStatus
#define gSerialRole   gSerialConnectionRole
#include "global.h"
#include "ram.h"
void Serial_ProbeConnection(void)
{
    vu16 *send = (vu16 *)0x0400012A;
    u16 busy;
    u16 control;
    *send = 0;
    control = *(vu16 *)0x04000128;
    if (!(control & 8)) {
        gSerialTransferStatus = -9;
    } else {
        busy = control & 0x80;
        if (busy) {
            gSerialTransferStatus = -4;
        } else {
            if (!(control & 4))
                gSerialConnectionRole = 0;
            else
                gSerialConnectionRole = -1;
            *send = busy;
        }
    }
}
