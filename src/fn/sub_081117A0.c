#include "global.h"
struct S {
    u8 f[0x1c];
    u32 a;
};
u32 ScriptProc_Start(u32, u16);
void Bg_SetupLayers(u32, u32, u32, u32, u32, u32, void *);
void Bg_SetPriority(s32, s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void PaletteFade_SetupBackground(struct S *s, u32 a, u32 b)
{
    u32 x;
    u32 r = ScriptProc_Start(0xC091, a);
    s->a = r;
    x = b;
    Bg_SetupLayers(2, 0, r, 0, 0, 1, &x);
    Bg_SetPriority(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
}
