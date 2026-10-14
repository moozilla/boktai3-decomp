#include "global.h"

u8 *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, u8 *, u32, u32, u32, u32 *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void CpuSet(const void *, void *, u32);

u32 ModifierMenu_LoadBackground(u8 *p, u32 x)
{
    u32 v;
    u32 z;
    v = x;
    *(u8 **)(p + 0x34) = ScriptProc_Start(0xC091, 0xCE15);
    z = 0;
    sub_082161B4(1, 0, *(u8 **)(p + 0x34), 0, z, 1, &v);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, z);
    *(u8 **)(p + 0x38) = ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    CpuSet(*(u8 **)(p + 0x38), (void *)0x03004FC0, 0x100);
    return 0;
}
