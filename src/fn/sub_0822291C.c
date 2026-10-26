#include "global.h"
#include "ram.h"
void Serial_ResetMultiplayer(void)
{
    gSerialTransferStatus = 0;
    gSerialConnectionRole = -1;
    *(vu16 *)0x04000208 = 0;
    *(vu16 *)0x04000200 &= 0xFF7F;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000134 = 0;
    *(vu16 *)0x04000128 = 0x2000;
    *(vu16 *)0x04000128 |= 3;
}
