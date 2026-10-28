#include "background_flash.h"
#include "render.h"
void BackgroundFlash_LoadPalette(void)
{
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0xE39B) + 0x194, gBgPaletteBank12, 0x04000008);
}
