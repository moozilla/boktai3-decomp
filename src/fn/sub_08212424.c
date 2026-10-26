#include "global.h"
#include "render.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Bg_SetPriority(u32, u32);
void ForgingBook_InitBackgrounds(u8 *p)
{
    u32 size = 16;
    u8 *res;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    res = ScriptProc_Start(0xc091, 0x3536);
    *(u8 **)(p + 0x18) = res;
    size = 15;
    Bg_SetupLayers(2, 0, res, 0, 0, 1, &size);
    Bg_SetPriority(3, 2);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(ScriptProc_Start(0x92b3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
}
