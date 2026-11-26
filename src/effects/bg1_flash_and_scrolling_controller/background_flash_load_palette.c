#include "effects/bg1_flash_and_scrolling_controller/background_flash.h"
#include "engine/shared/render.h"
void BackgroundFlash_LoadPalette(void)
{
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0xE39B) + 0x194, gBgPaletteBank12, 0x04000008);
}
