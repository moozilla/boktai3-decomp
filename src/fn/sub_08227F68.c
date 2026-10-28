#include "global.h"
#include "render.h"
s32 Script_GetKeywordValue(s32, s32);
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, s32 *);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
/* Script command: load map `b` with value `i`, and palette `p` if given. */
void Bg_LoadMapFromScript(void)
{
    s32 map, value, pal;
    map = Script_GetKeywordValue('b', 0);
    value = Script_GetKeywordValue('i', 0);
    pal = Script_GetKeywordValue('p', 0);
    Bg_SetupLayers(0, 0, ScriptProc_Start(0xC091, map), 0, 0, 1, &value);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    if (pal)
        CpuSet((u8 *)ScriptProc_Start(0x92B3, pal) + 0x1B4, gBgPaletteBank13, 0x30);
}
