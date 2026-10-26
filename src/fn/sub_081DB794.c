// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: background, labels, animations and menu selection initialization.
// fix.py best (sweep after the inlined-insn fix)
struct Rect {
    u16 x, y, width, height;
};
extern u32 gUnk_020002C0[];
extern u16 gUnk_03004FC0[];
void Bg_ClearTilemap(s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void *ScriptProc_Start(s32, s32);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, void *);
void CpuSet(const void *, void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, s32);
void Menu_SetTextRect(void *, void *);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, u32, s32);
void Menu_StartFadeIn(s32);
struct OptionsHalfword {
    u16 value;
};
#define W(o) (*(u32 *)(p + (o)))
s32 Options_InitScreen(u8 *arg)
{
    u8 *p = arg;
    u32 zero;
    u32 one;
    void *resource;
    void *background;
    void *palette;
    register void *graphics asm("r4");
    void *sprite;
    void *sprite2;
    void *tiles;
    s32 layer;
    struct Rect rect;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    background = ScriptProc_Start(0xC091, 0x3536);
    layer = 9;
    Bg_SetupLayers(2, 0, background, 0, zero, one = 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    palette = ScriptProc_Start(0x92B3, 0x204);
    CpuSet(((u8 *)palette) + 0x14, gUnk_03004FC0, 0x100);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    graphics = p + 0x84;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(p + 0x24, graphics, 0x3A, 0x10, zero, zero, zero, zero);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    graphics = p + 0x10C;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(p + 0xAC, graphics, 0x8A, 0x10, zero, zero, zero, zero);
    graphics = p + 0x194;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(p + 0x134, graphics, 0x47, 0x10, zero, zero, zero, zero);
    graphics = p + 0x21C;
    ResourceHeader_Relocate(graphics, resource);
    sprite = p + 0x1BC;
    Widget_InitLinked(sprite, graphics, 0x45, 0x10, zero, zero, zero, zero);
    rect.x = 0x40;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(sprite, &rect);
    graphics = p + 0x2A4;
    ResourceHeader_Relocate(p + 0x2A4, resource);
    sprite2 = p + 0x244;
    Widget_InitLinked(sprite2, p + 0x2A4, 0x46, 0x10, zero, zero, zero, zero);
    rect.x = 0xD8;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(sprite2, &rect);
    *((u32 *)(p + 0x13C)) |= one;
    *((u32 *)(p + 0x1C4)) |= one;
    *((u32 *)(p + 0x24C)) |= one;
    tiles = (void *)(*((u32 *)(p + 0x2DC)) = *((u32 *)(p + 0x17C)));
    graphics = p + 0x2E0;
    CpuSet(tiles, graphics, 0x10);
    *((u32 *)(p + 0x17C)) = (u32)graphics;
    ((struct OptionsHalfword *)(p + 0x16E))->value = 0xE010;
    Menu_EraseRect(4, 4, 24, 6);
    Menu_EraseRect(4, 12, 4, 2);
    Menu_DrawStringRef(4, 4, gUnk_020002C0[0], 0);
    Menu_DrawStringRef(4, 8, gUnk_020002C0[0], 1);
    Menu_DrawStringRef(4, 12, gUnk_020002C0[0], 2);
    *((u32 *)(p + 0x20)) = gUnk_020002C0[8];
    Menu_EraseRect(9, 12, 18, 2);
    Menu_DrawStringRef(9, 12, gUnk_020002C0[1], *((u32 *)(p + 0x20)));
    if ((gUnk_020002C0[7] == 0) || (gUnk_020002C0[6] == 2))
        *((u32 *)(p + 0x2D8)) = 3;
    else
        *((u32 *)(p + 0x2D8)) = zero;
    Menu_StartFadeIn(*((u32 *)(p + 0x2D8)));
    *((u32 *)(p + 0x1C)) = gUnk_020002C0[6];
    return 0;
}
