#include "global.h"
#include "render.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(u8 *, void *);
void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
void Menu_StartFadeIn(u32);
void ScriptPrompt_ResetDisplay(u8 *);
extern u32 gScreenParams[];
u32 TextWindow_Create(u32, u32, u32, u32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(u32);
void Palette_SetBackdropColor(u32);
u32 ScreenMessage_Initialize(u8 *s)
{
    u32 zero, one, value;
    void *a, *p, *resource;
    u8 *obj;
    u8 *obj2;
    u32 *global;
    u32 *save;
    u8 *dest;
    u32 old;
    u32 *sound;
    u32 result;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    a = ScriptProc_Start(0xC091, 0xA597);
    value = 3;
    Bg_SetupLayers(2, 0, a, 0, zero, one = 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = (u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x14;
    CpuSet(p, gBgPaletteBuffer, 0x100);
    global = gScreenParams;
    if (global[2] == 1) {
        u32 x, y;
        resource = ScriptProc_Start(0xCB05, 0x530D);
        obj = s + 0x7C;
        ResourceHeader_Relocate(obj, resource);
        Widget_InitLinked(s + 0x1C, obj, 0x4B, 0x10, zero, zero, zero, zero);
        obj2 = s + 0x104;
        ResourceHeader_Relocate(obj2, resource);
        Widget_InitLinked(s + 0xA4, obj2, 0x4C, 0x10, zero, zero, zero, zero);
        x = 0xFFF8;
        *(u16 *)(s + 0x3C) = x;
        *(u16 *)(s + 0x3E) = y = 0xFFC0;
        *(u16 *)(s + 0xC4) = x;
        *(u16 *)(s + 0xC6) = y;
        *(u32 *)(s + 0x24) |= one;
        *(u32 *)(s + 0xAC) |= one;
        save = (u32 *)(s + 0x144);
        old = *(u32 *)(s + 0x64);
        *save = old;
        dest = s + 0x148;
        CpuSet((void *)old, dest, 0x10);
        *(void **)(s + 0x64) = dest;
        *(void **)(s + 0xEC) = dest;
    }
    result = TextWindow_Create(1, 6, 28, 6);
    sound = (u32 *)(s + 0x140);
    *sound = result;
    TextWindow_SetScript(result, global[0]);
    TextWindow_Refresh(*sound, global[1]);
    TextWindow_StartPlayback(*sound);
    *(u32 *)(s + 0x138) = zero;
    Menu_StartFadeIn(0);
    *(u32 *)(s + 0x13C) = one;
    Palette_SetBackdropColor(0x73FF);
    return 0;
}
