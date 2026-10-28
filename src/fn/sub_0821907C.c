#include "global.h"
#include "engine.h"
#include "render.h"
/* TextWindow_LoadFramePalette: Selects one of two resources and copies 32 palette colors into RAM; tentative frame palette role */
#define TextWindow_LoadFramePalette sub_0821907C

void sub_0821907C(u32 mode)
{
    u32 key;
    u8 *res;
    switch (mode) {
        case 0:
        default:
            key = 0x529c;
            break;
        case 1:
            key = 0x539c;
            break;
    }
    res = ScriptProc_Start(0x92b3, key);
    CpuSet(res + 0x1d4, gBgPaletteBank14, 0x04000010);
}
