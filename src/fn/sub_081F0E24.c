#include "global.h"
void Bg_ClearTilemap(u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void sub_08215A74(u32, u32);
void sub_081F0E24(u8 *s)
{
    s32 kind = *(s16 *)(s + 0x5EE);
    struct Scratch {
        u32 fill, value;
    } scratch;
    u32 zero, key;
    void *p, *q, *r;
    u8 *dst;
    u32 *fill;
    int ctl;
    ctl = 0x05000002;
    if (kind) {
        Bg_ClearTilemap(1);
        *(void **)(s + 0x3A4) = (p = ScriptProc_Start(0xC091, 0x42FF));
        scratch.value = 0;
        Bg_SetupLayers(3, 0, p, 0, 0, 1, &scratch.value);
        BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0x60);
    } else {
        Bg_ClearTilemap(3);
        *(void **)(s + 0x3A4) = (p = ScriptProc_Start(0xC091, 0x42FF));
        scratch.value = kind;
        Bg_SetupLayers(2, 0, p, 0, kind, 1, &scratch.value);
        sub_08215A74(3, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0x60);
    }
    key = 0x92B3;
    q = (u8 *)ScriptProc_Start(key, 0xB499) + 0x94;
    CpuSet(q, s + 0x408, 0x40);
    r = (u8 *)ScriptProc_Start(key, 0xDDFB) + 0x94;
    CpuSet(r, s + 0x488, 0x40);
    *(u32 *)(s + 0x268) = 0xFFF9C000;
    *(u32 *)(s + 0x26C) = (zero = 0);
    *(u32 *)(s + 0x270) = 0x1C000;
    dst = s + 0x278;
    scratch.fill = zero;
    fill = &scratch.fill;
    CpuSet(fill, dst, ctl);
    *(u32 *)(s + 0x284) = zero;
    *(u32 *)(s + 0x280) = 0x5000;
    *(u32 *)(s + 0x288) = zero;
}
