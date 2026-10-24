#include "global.h"
#include "gba/syscall.h"

// Sleeps in the BIOS Stop state until the keypad interrupt (KEYCNT 0xC304: L+R+Select) wakes it.
void InterruptsAndVideo_WaitForVBlank(void);
void SoundBiasReset(void);
void SoundBiasSet(void);
extern u32 gUnk_03005404;

void Power_EnterSleep(void)
{
    *(vu16 *)0x04000000 |= 0x80;
    InterruptsAndVideo_WaitForVBlank();
    InterruptsAndVideo_WaitForVBlank();
    *(vu16 *)0x04000132 = 0xC304;
    SoundBiasReset();
    SystemCall(3);
    SoundBiasSet();
    gUnk_03005404 = 1;
    *(vu16 *)0x04000000 &= 0xFF7F;
    *(vu16 *)0x04000132 = 0;
    InterruptsAndVideo_WaitForVBlank();
}
