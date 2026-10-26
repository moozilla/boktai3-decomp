#include "global.h"
#include "render.h"
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void RegistryF246_InitBackgroundResources(u8 *s)
{
    u32 value;
    void *p;
    void *q;
    p = ScriptProc_Start(0xC091, 0xCE15);
    *(void **)(s + 0x18) = p;
    value = 2;
    Bg_SetupLayers(2, 0, p, 0, 0, 1, &value);
    q = (u8 *)ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    *(void **)(s + 0x1C) = q;
    CpuSet(q, gBgPaletteBuffer, 0x100);
}
