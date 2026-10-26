#include "global.h"
#include "render.h"
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
s32 TimedPresentation_Initialize(u8 *s)
{
    u32 zero, value;
    void *p, *q;
    *(u32 *)(s + 0x24) = 4;
    *(u32 *)(s + 0x28) = 4;
    *(u32 *)(s + 0x2C) = 300;
    if (Script_SeekToKeyword(0x66)) {
        *(u32 *)(s + 0x24) = Script_GetValue();
        *(u32 *)(s + 0x28) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64))
        *(u32 *)(s + 0x2C) = Script_GetValue();
    if (Script_SeekToKeyword(0x74))
        *(u32 *)(s + 0x30) = Script_GetValue();
    if (Script_SeekToKeyword(0x65))
        *(u32 *)(s + 0x34) = Script_GetValue();
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero = 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    p = ScriptProc_Start(0xC091, 0x52AE);
    value = *(u32 *)(s + 0x30);
    Bg_SetupLayers(2, 0, p, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    q = (u8 *)ScriptProc_Start(0x92B3, 0xD985) + 0x14;
    CpuSet(q, gBgPaletteBuffer, 0x100);
    Menu_StartFadeIn(*(u32 *)(s + 0x24));
    return 0;
}
