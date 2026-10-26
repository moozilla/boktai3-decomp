#include "global.h"
#include "engine.h"
#include "render.h"
/* WirelessStatusMenu_LoadPrimaryBackground: Loads the BG0 tilemap resource and its palette into the menu. */
#define WirelessStatusMenu_LoadPrimaryBackground sub_08036E60

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void sub_08036E60(u8 *p, u32 index)
{
    u8 *res;
    *(void **)(p + 0x24) = ScriptProc_Start(0xc091, 0xa597);
    Bg_SetupLayers(0, 0, *(void **)(p + 0x24), 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    res = ScriptProc_Start(0x92b3, 0x81b);
    CpuSet(res + 20, gBgPaletteBuffer, 0x100);
}
