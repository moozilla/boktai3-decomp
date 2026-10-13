#include "global.h"
void *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, u32 *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ResourceHeader_Relocate(void *, void *);
void sub_0821983C(u8 *, void *, u32, u32, u32, u32, u32, void *);
extern u8 gUnk_0824F6F8[];
u32 sub_0819A5AC(u8 *s)
{
    struct Blob {
        u32 words[8];
    } resource;
    struct Pair {
        u32 a, b;
    } values;
    void *p = ScriptProc_Start(0xC091, 0xCF15);
    u32 zero;

    values.a = 3;
    values.b = 4;
    sub_082161B4(1, 0, p, 0, zero = 0, 2, &values.a);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = ScriptProc_Start(0xCB05, 0x7C8B);
    resource = *(struct Blob *)p;
    ResourceHeader_Relocate(&resource, p);
    sub_0821983C(s + 0x18, &resource, 1, 0x10, zero, zero, 0x3C, gUnk_0824F6F8);
    s[0x78] = 1;
    s[0x79] = zero;
    return 0;
}
