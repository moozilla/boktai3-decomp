#include "global.h"
#include "render.h"
void Bg_SetPriority(u32, u32);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 Collection_GetPageEntryCount(s32);
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
void Collection_LoadPageBackground(u8 *p)
{
    u32 size;
    s32 max, count, row;
    struct BackgroundLayer *tile;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    {
        u8 *res = ScriptProc_Start(0xc091, 0x3536);
        *(u8 **)(p + 0x18) = res;
        size = 16;
        Bg_SetupLayers(2, 0, res, 0, 0, 1, &size);
    }
    Bg_SetPriority(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(ScriptProc_Start(0x92b3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
    count = 0;
    max = Collection_GetPageEntryCount(*(s16 *)(p + 0xb80));
    row = 0;
    tile = &gBgRecords[3];
    do {
        int next = row + 1;
        int x = 3, y = row * 5, i = 4;
        do {
            tile->width = 16;
            tile->x = 0;
            tile->y = 0;
            count++;
            if (count > max)
                sub_08215EB4(3, x, y + 3, 4, 4, 1, 0, 2);
            x += 5;
            i--;
        } while (i >= 0);
        row = next;
    } while (row <= 1);
}
