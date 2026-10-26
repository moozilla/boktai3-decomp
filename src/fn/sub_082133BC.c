#include "global.h"
#include "render.h"
struct Tile {
    u8 p0[0x18];
    u16 h18;
    u8 p1A[6];
    u16 h20, h22;
};
extern struct Tile gUnk_03004CC0;
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 CollectionBook_GetPageEntryCount(s32);
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
void ForgingBook_SetupPageBackground(u8 *p)
{
    u32 size = 16;
    s32 max, count, row;
    struct Tile *tile;
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
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(ScriptProc_Start(0x92b3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
    count = 0;
    max = CollectionBook_GetPageEntryCount(*(s16 *)(p + 0x6f4));
    row = 0;
    tile = &gUnk_03004CC0;
    do {
        int next = row + 1;
        int x = 3, y = row * 5, i = 4;
        do {
            tile->h18 = 16;
            tile->h20 = 0;
            tile->h22 = 0;
            count++;
            if (count > max)
                sub_08215EB4(3, x, y + 3, 4, 4, 1, 0, 2);
            x += 5;
            i--;
        } while (i >= 0);
        row = next;
    } while (row <= 1);
    {
        s32 zero = 0, two = 2;
        sub_08215EB4(3, 0, 0, 2, two, two, zero, zero);
        sub_08215EB4(3, 28, 0, 2, two, two, zero, zero);
    }
}
