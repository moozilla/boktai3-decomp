// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

struct ResourceHeader {
    u32 words[8];
};
extern u32 gUnk_02000340[];
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);
void sub_081DE630(void *);
void Password_LoadDate(void *);
void AnimPanel_UpdateDigits(void *);
void Menu_StartFadeIn(u32);
s32 Time_InitializeDateMenu(u8 *s)
{
    u32 zero;
    register u32 one asm("r10");
    u32 bank;
    register void *resource asm("r6");
    register u8 *first asm("r4");
    u8 *second;
    u8 *obj;
    u32 value;
    void *bg;
    u32 *palette;
    u8 *copy;
    u16 bits;
    u16 *half;
    u32 off1;
    u32 off2;
    u32 off3;
    u32 off5;
    *(u32 *)(s + 0x6E4) = zero = 0;
    *(u32 *)(s + 0x6D8) = zero;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    bg = ScriptProc_Start(0xC091, 0x3536);
    value = 2;
    Bg_SetupLayers(2, 0, bg, 0, zero, one = 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
    bank = 0xCB05;
    resource = ScriptProc_Start(bank, 0xE2AB);
    first = s + 0x78;
    ResourceHeader_Relocate(first, resource);
    *(struct ResourceHeader *)(s + 0x6B8) = *(struct ResourceHeader *)first;
    Widget_InitLinked(s + 0x18, first, 0x34, 0x10, zero, zero, zero, zero);
    second = s + 0x100;
    ResourceHeader_Relocate(second, resource);
    Widget_InitLinked(s + 0xA0, second, 0x26, 0x10, zero, zero, zero, zero);
    first = s + 0x188;
    ResourceHeader_Relocate(first, resource);
    Widget_InitLinked(s + 0x128, first, 0x27, 0x10, zero, zero, zero, zero);
    *(u32 *)(s + 0xA8) |= one;
    *(u32 *)(s + 0x130) |= one;
    {
        u32 *save = (u32 *)(s + 0x718);
        u32 old;
        palette = (u32 *)(s + 0xE8);
        old = *palette;
        *save = old;
        off1 = 0x71C;
        copy = s + off1;
        CpuSet((void *)old, copy, 0x10);
        *palette = (u32)copy;
    }
    off2 = 0x170;
    *(void **)(s + off2) = copy;
    half = (u16 *)(s + 0xDA);
    bits = 0xE000;
    *half = bits;
    off3 = 0x162;
    half = (u16 *)(s + off3);
    *half = bits + 1;
    resource = ScriptProc_Start(bank, 0x530D);
    off5 = 0x210;
    obj = s + off5;
    ResourceHeader_Relocate(obj, resource);
    off1 = 0x1B0;
    Widget_InitLinked(s + off1, obj, 0x80, 0x10, zero, zero, zero, zero);
    sub_081DE630(s);
    Password_LoadDate(s);
    AnimPanel_UpdateDigits(s);
    if (gUnk_02000340[4] == 1) {
        off3 = 0x6E0;
        *(u32 *)(s + off3) = zero;
    } else {
        u32 offset = 0x6E0;
        *(u32 *)(s + offset) = 3;
    }
    Menu_StartFadeIn(*(u32 *)(s + 0x6E0));
    return 0;
}
