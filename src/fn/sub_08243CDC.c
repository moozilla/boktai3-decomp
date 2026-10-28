#include "global.h"
#include "ram.h"
#include "gba/io_reg.h"
void InterruptsAndVideo_OnUnusedInterrupt(void);
void Solar_ShutdownTimer(void)
{
    u16 ie;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 0;
    *(vu16 *)0x0400010E = 0;
    *(vu16 *)0x0400010C = 0;
    gSerialInterruptState[2] = InterruptsAndVideo_OnUnusedInterrupt;
    *(vu16 *)0x080000C8 = 0;
    REG_IE = ie & 0xFFBF;
    REG_IME = 1;
}
