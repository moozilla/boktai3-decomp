#include "global.h"
/* Initialize the controller's background layers and palette. */
extern u8 gUnk_03004FC0[];
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, u32 *);
void sub_08215A74(u32, u32);
void sub_081EA7F0(u8 *p)
{
    void *resource;
    u32 layer;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    resource = ScriptProc_Start(0xc091, 0xc5a7);
    *(void **)(p + 0x720) = resource;
    layer = 0;
    sub_082161B4(1, 16, resource, 0, 0, 1, &layer);
    sub_08215A74(2, 2);
    layer = 1;
    sub_082161B4(2, 0, resource, 0, 0, 1, &layer);
    sub_08215A74(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet((u8 *)ScriptProc_Start(0x92b3, 0x91d) + 20, gUnk_03004FC0, 256);
}
