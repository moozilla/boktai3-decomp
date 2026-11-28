#include "global.h"
#include "gba/syscall.h"
#include "engine/runtime.h"
#include "gba/io_reg.h"

// Sleeps in the BIOS Stop state until the keypad interrupt (KEYCNT 0xC304: L+R+Select) wakes it.
void InterruptsAndVideo_WaitForVBlank(void);
void SoundBiasReset(void);
void SoundBiasSet(void);

void Power_EnterSleep(void)
{
    REG_DISPCNT |= DISPCNT_FORCED_BLANK;
    InterruptsAndVideo_WaitForVBlank();
    InterruptsAndVideo_WaitForVBlank();
    REG_KEYCNT = 0xC304;
    SoundBiasReset();
    SystemCall(3);
    SoundBiasSet();
    gSleepResumePending = 1;
    REG_DISPCNT &= ~DISPCNT_FORCED_BLANK;
    REG_KEYCNT = 0;
    InterruptsAndVideo_WaitForVBlank();
}
