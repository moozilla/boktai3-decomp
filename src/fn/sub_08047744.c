#include "background_flash.h"
#define BackgroundFlash_LoadPalette sub_08047744
extern u8 gUnk_03005140[];
void BackgroundFlash_LoadPalette(void)
{
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0xE39B) + 0x194, gUnk_03005140, 0x04000008);
}
