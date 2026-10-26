#include "global.h"
#include "render.h"

u16 *ScriptProc_Start(u32, u32);
void sub_082171C0(u16 *, u32);

void AnimData_InitPalettePool(void)
{
    u16 *p = ScriptProc_Start(0x9B1B, 0xC5E9);
    gPaletteBankCount = p[0];
    gPaletteBankData = p + 2;
    sub_082171C0(p + 2, 8);
}
