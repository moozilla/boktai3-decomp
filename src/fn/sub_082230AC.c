#include "global.h"
#include "ram.h"
void InterruptsAndVideo_WaitForVBlank(void);
void Serial_ResetMultiplayer_082230AC(void)
{
    gSerialTransferStatus = 0;
    gSerialConnectionRole = -1;
    *(vu16 *)0x04000208 = 0;
    *(vu16 *)0x04000200 &= 0xFF7F;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000202 = 0x80;
    *(vu16 *)0x04000134 = 0x8000;
    *(vu16 *)0x04000128 = 0;
    InterruptsAndVideo_WaitForVBlank();
    InterruptsAndVideo_WaitForVBlank();
    *(vu16 *)0x04000134 = 0;
    *(vu16 *)0x04000128 = 0x2000;
    *(vu16 *)0x04000128 |= 3;
    InterruptsAndVideo_WaitForVBlank();
}
