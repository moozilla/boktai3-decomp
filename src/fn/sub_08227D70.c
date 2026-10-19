#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.9%
// foundry draft (worker best 91.9%); what differs is mostly register allocation and instruction order
#define System_EnterSleep      sub_08227D70
#define System_WaitFrame       InterruptsAndVideo_WaitForVBlank
#define gDisplayRefreshPending gUnk_03005404

void System_WaitFrame(void);
void SoundBiasReset(void);
void SoundBiasSet(void);
extern u32 gDisplayRefreshPending;

void sub_08227D70(void)
{
    *(vu16 *)0x04000000 |= 0x80;
    System_WaitFrame();
    System_WaitFrame();
    *(vu16 *)0x04000132 = 0xC304;
    SoundBiasReset();
    /* Unrecovered BIOS Stop (svc 3): requires an inline SWI facility.
     * Foundry forbids assembly, and this compiler has no SWI intrinsic. */
    SoundBiasSet();
    gDisplayRefreshPending = 1;
    *(vu16 *)0x04000000 &= 0xFF7F;
    *(vu16 *)0x04000132 = 0;
    System_WaitFrame();
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08227D70);
#endif
