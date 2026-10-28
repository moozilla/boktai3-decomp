#include "global.h"
#include "render.h"
u8 *ScriptProc_Start(int, int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
int ResultMenu_ConfigureLayers(int value, int mode)
{
    u8 *resource = ScriptProc_Start(0xc091, 0xa597);
    int result;
    if (mode) {
        Bg_SetupLayers(3, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0);
        result = 1;
    } else {
        Bg_SetupLayers(0, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        result = 0;
    }
    {
        u8 *palette = ScriptProc_Start(0x92b3, 0x81b);
        CpuSet(palette + 0x1b4, gBgPaletteBank13, 0x30);
    }
    return result;
}
