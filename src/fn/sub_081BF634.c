#include "global.h"
#include "render.h"
u8 *ScriptProc_Start(u32, u32);
void Storage_LoadDisplayResources(u8 *p)
{
    u8 *r = ScriptProc_Start(0x92B3, 0x81B);
    *(u8 **)(p + 0x1730) = r + 0x1b4;
    CpuSet(r + 0x1b4, gBgPaletteBank13, 0x04000018);
    *(u8 *)(p + 0x17C4) = 0;
}
