#include "global.h"
#include "render.h"
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void LinkConnectionMenu_LoadBackground(u8 *s)
{
    u32 value;
    void *p;
    void *q;
    p = ScriptProc_Start(0xC091, 0xA597);
    *(void **)(s + 0x18) = p;
    value = 3;
    Bg_SetupLayers(2, 0, p, 0, 0, 1, &value);
    q = (u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x14;
    *(void **)(s + 0x1C) = q;
    CpuSet(q, gBgPaletteBuffer, 0x100);
}
