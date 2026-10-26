#include "global.h"
#include "render.h"
extern u32 gUnk_02000358[];
void AnimPanel_LoadSelections(u8 *);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(u8 *, void *);
void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
void Menu_StartFadeIn(u32);
void sub_081DF1D8(u8 *);
void Password_BuildLabels(u8 *);
u32 Password_InitializeEntryMenu(u8 *s)
{
    u8 *obj_2;
    u32 *flags = (u32 *)(s + 0x34C);
    u32 zero;
    u32 size;
    u32 one;
    u32 value;
    void *p, *a;
    void *q;
    u8 *obj;
    u8 *first;
    u32 bank;
    s32 offA;
    s32 offB;
    u32 *oneptr;
    u32 *ptr;
    u32 *initial;
    u16 *halfA;
    u16 *halfB;
    u16 bitsA;
    u16 bitsB;
    u8 *temp;
    *flags = (zero = 0);
    offA = 0x348;
    *(u32 *)(s + offA) = zero;
    offB = 0x350;
    *(u32 *)(s + offB) = zero;
    offA += 0x10;
    temp = s + offA;
    *(u16 *)temp = zero;
    offB += 0xA;
    *(u16 *)(s + offB) = zero;
    oneptr = (u32 *)(s + 0x35C);
    offB += 6;
    initial = (u32 *)(s + offB);
    *initial = (one = 1);
    *oneptr = one;
    *(u16 *)(s + 0x364) = zero;
    AnimPanel_LoadSelections(s);
    if (gUnk_02000358[6]) {
        *flags = 0x41;
    }
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    a = ScriptProc_Start(0xC091, 0x3536);
    *(void **)(s + 0x354) = a;
    value = 8;
    Bg_SetupLayers(2, 0, a, 0, zero, one, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    q = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14;
    CpuSet(q, gBgPaletteBuffer, size = 0x100);
    bank = 0xCB05;
    p = ScriptProc_Start(bank, 0xE2AB);
    first = s + 0x78;
    ResourceHeader_Relocate(first, p);
    Widget_InitLinked(s + 0x18, first, 0x39, 0x10, zero, zero, zero, zero);
    obj_2 = s + size;
    ResourceHeader_Relocate(obj_2, p);
    Widget_InitLinked(s + 0xA0, obj_2, 0x16, 0x10, zero, zero, zero, zero);
    obj = s + 0x188;
    ResourceHeader_Relocate(obj, p);
    Widget_InitLinked(s + 0x128, obj, 0x18, 0x10, zero, zero, zero, zero);
    obj = s + 0x210;
    ResourceHeader_Relocate(obj, p);
    Widget_InitLinked(s + 0x1B0, obj, 0x32, 0x10, zero, zero, zero, zero);
    {
        u32 *save = (u32 *)(s + 0x7D0);
        u32 old;
        ptr = (u32 *)(first + 0x70);
        old = *ptr;
        *save = old;
        obj = s + 0x7D8;
        CpuSet((void *)old, obj, 0x10);
        *ptr = (u32)obj;
    }
    *(void **)(s + 0x170) = obj;
    halfA = (u16 *)(s + 0xDA);
    bitsA = 0xE000;
    *halfA = bitsA;
    halfA = (u16 *)(s + 0x162);
    {
        u32 palette = 0xE002;
        *halfA = palette;
    }
    p = ScriptProc_Start(bank, 0x530D);
    offA = 0x298;
    obj = s + offA;
    ResourceHeader_Relocate(obj, p);
    offB = 0x238;
    Widget_InitLinked(s + offB, obj, 0x8C, 0x10, one, zero, zero, zero);
    offB = 0x320;
    obj = s + offB;
    ResourceHeader_Relocate(obj, p);
    Widget_InitLinked(s + 0x2C0, obj, 0x8D, 0x10, zero, zero, zero, zero);
    *(u32 *)(s + 0x240) &= ~1;
    *(u32 *)(s + 0x2C8) |= one;
    {
        u32 *save = (u32 *)(s + 0x7D4);
        u32 old;
        ptr = (u32 *)(s + 0x308);
        old = *ptr;
        *save = old;
        obj = s + 0x7F8;
        CpuSet((void *)old, obj, 0x10);
        *ptr = (u32)obj;
    }
    halfB = (u16 *)(s + 0x2FA);
    bitsB = 0xE010;
    *halfB = bitsB;
    *(u32 *)(s + 0x36C) = zero;
    Menu_StartFadeIn(0);
    sub_081DF1D8(s);
    Password_BuildLabels(s);
    return 0;
}
