#include "global.h"
#include "render.h"
#include "menu_parameters.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(u8 *, void *);
void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
void Menu_StartFadeIn(u32);
void ScriptPrompt_ResetDisplay(u8 *);
void MessageWindow_SetPalette(u32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_ShowMessage(u32);
void MessageWindow_SetBounds(u32, u32, u32, u32);
void MessageWindow_SetDisplayMode(u32);
u32 ScreenParams2E8_InitScreen(u8 *s)
{
    u32 zero, one, value;
    void *a, *p, *resource;
    u8 *obj, *obj2, *obj3;
    struct MessageMenuParameters *global;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    a = ScriptProc_Start(0xC091, 0x3536);
    *(void **)(s + 0x18) = a;
    value = 6;
    Bg_SetupLayers(2, 0, a, 0, zero, one = 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14;
    CpuSet(p, gBgPaletteBuffer, 0x100);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    obj = s + 0xA4;
    ResourceHeader_Relocate(obj, resource);
    Widget_InitLinked(s + 0x44, obj, 0x38, 0x10, zero, zero, zero, zero);
    obj2 = s + 0x12C;
    ResourceHeader_Relocate(obj2, resource);
    Widget_InitLinked(s + 0xCC, obj2, 0x29, 0x10, zero, zero, zero, zero);
    obj3 = s + 0x1B4;
    ResourceHeader_Relocate(obj3, resource);
    Widget_InitLinked(s + 0x154, obj3, 0x2A, 0x10, zero, zero, zero, zero);
    *(u32 *)(s + 0xD4) &= ~1;
    *(u32 *)(s + 0x15C) |= one;
    MessageWindow_SetPalette(0xC486);
    global = &gMessageMenuParameters;
    MessageWindow_SetMessageTable(global->lBlock);
    MessageWindow_ShowMessage(0);
    MessageWindow_SetBounds(2, 3, 0x20, 6);
    MessageWindow_SetDisplayMode(1);
    *(u32 *)(s + 0x28) = one;
    *(u32 *)(s + 0x24) = one;
    if (global->f == 1)
        *(u32 *)(s + 0x40) = 3;
    else
        *(u32 *)(s + 0x40) = zero;
    Menu_StartFadeIn(*(u32 *)(s + 0x40));
    return *(u32 *)(s + 0x40) = 0;
}
