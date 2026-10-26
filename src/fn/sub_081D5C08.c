#include "global.h"
#include "render.h"
extern u8 gUnk_03004CC0[];
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Bg_SetPriority(u32, u32);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void sub_08215EB4(u32, u32, u32, u32, u32, u32, u32, u32);
u32 MonsterBook_SetupPageBackgrounds(u8 *s, u32 value)
{
    u32 mask = 0xF00;
    u16 *reg = &gDispCntShadow;
    void *p, *q;
    u32 color;
    u16 *dst;
    s32 i;
    u32 zero, one;
    struct Fields {
        u8 pad[0x18];
        u16 first;
        u8 pad2[6];
        u16 second, third;
    } *fields;
    mask |= *reg;
    *reg = mask;
    p = ScriptProc_Start(0xC091, 0x3536);
    *(void **)(s + 0x18) = p;
    Bg_SetupLayers(1, 0, p, 0, 0, 1, &value);
    Bg_SetPriority(2, 0);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    q = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14;
    *(void **)(s + 0x1C) = q;
    dst = (u16 *)gBgPaletteBuffer;
    CpuSet(q, dst, 0x30);
    color = 0x73FF;
    i = 15;
    dst += 0x4F;
    for (; i >= 0; i--) {
        *dst = color;
        dst--;
    }
    zero = 0;
    Bg_SetupLayers(2, 0, *(void **)(s + 0x18), 0, zero, one = 1, &value);
    fields = (struct Fields *)gUnk_03004CC0;
    fields->first = 0x10;
    fields->second = zero;
    fields->third = zero;
    Bg_SetPriority(3, 3);
    sub_08215EB4(3, 4, 6, 8, 9, one, zero, 4);
    return 0;
}
