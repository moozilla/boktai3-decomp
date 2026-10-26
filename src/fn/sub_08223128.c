#include "global.h"
#include "ram.h"

void Serial_CheckMultiplayerReady(void)
{
    u32 status;
    gSerialConnectionRole = -1;
    gSerialTransferStatus = 0;
    status = *(vu16 *)0x04000128;
    if ((u16)(status & 0x88) != 8) {
        gSerialTransferStatus = -1;
    } else {
        if ((u16)(status & 4) == 0)
            gSerialConnectionRole = 0;
        else
            gSerialConnectionRole = -1;
        *(vu16 *)0x0400012A = 0x4000;
    }
}
