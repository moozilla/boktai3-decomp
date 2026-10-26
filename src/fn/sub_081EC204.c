#include "global.h"
#include "render.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(u8 *, void *);
void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
u32 Menu_StartFadeIn(u32);
void ScriptPrompt_ResetDisplay(u8 *);
u32 Results_Initialize(u8 *s)
{
    u32 zero;
    u32 value;
    u32 bank;
    u32 size;
    void *a;
    void *p;
    void *q;
    u32 old;
    u8 *obj;
    u8 *obj2;
    u8 *obj3;
    u32 *ptr;
    u32 *save;
    u8 *dest;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    a = ScriptProc_Start(0xC091, 0x3536);
    value = 11;
    Bg_SetupLayers(2, 0, a, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14;
    CpuSet(p, gBgPaletteBuffer, size = 0x100);
    bank = 0xCB05;
    q = ScriptProc_Start(bank, 0xE2AB);
    obj = s + 0x78;
    ResourceHeader_Relocate(obj, q);
    Widget_InitLinked(s + 0x18, obj, 0x3C, 0x10, zero, zero, zero, zero);
    q = ScriptProc_Start(bank, 0x530D);
    obj2 = s + size;
    ResourceHeader_Relocate(obj2, q);
    Widget_InitLinked(s + 0xA0, obj2, 0x80, 0x10, zero, zero, zero, zero);
    *(u32 *)(s + 0x134) = 3;
    return Menu_StartFadeIn(3);
}
