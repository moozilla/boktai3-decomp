#include "global.h"
#include "render.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Bg_SetLayerMap(u32, void *, u32);
void Presentation_InitializeBackground(void)
{
    u32 id = 0xA597, pal = 0x81B;
    u32 zero, one, value;
    void *p;
    u16 *src, *dest;
    s32 i, j;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero = 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    one = 1;
    {
        u16 *g = &gDispCntShadow;
        u32 a = ~1;
        u32 b = *g;
        a &= b;
        a &= ~2;
        *g = a;
    }
    p = ScriptProc_Start(0xC091, id);
    value = zero;
    Bg_SetupLayers(0, 0, p, 0, zero, one, &value);
    Bg_SetLayerMap(0, p, 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    src = (u16 *)((u8 *)ScriptProc_Start(0x92B3, pal) + 0x1B4);
    for (i = 13; i <= 15;) {
        u32 offset = i * 32;
        s32 next = i + 1;
        dest = (u16 *)(((u8 *)gBgPaletteBuffer) + offset);
        for (j = 15; j >= 0; j--) {
            *dest = *src;
            src++;
            dest++;
        }
        i = next;
    }
}
