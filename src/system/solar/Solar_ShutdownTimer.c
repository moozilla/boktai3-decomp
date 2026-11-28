#include "global.h"
#include "engine/ram.h"
#include "gba/io_reg.h"
void InterruptsAndVideo_OnUnusedInterrupt(void);
void Solar_ShutdownTimer(void)
{
    u16 ie;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 0;
    REG_TM3CNT_H = 0;
    REG_TM3CNT_L = 0;
    gSerialInterruptState[2] = InterruptsAndVideo_OnUnusedInterrupt;
    *(vu16 *)0x080000C8 = 0;
    REG_IE = ie & ~INTR_FLAG_TIMER3;
    REG_IME = 1;
}
