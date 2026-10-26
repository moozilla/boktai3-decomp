#include "global.h"
#include "render.h"
/* Tentative screen family: keeps two palettes and copies the first to shadow RAM. */
struct PaletteScreen {
    u8 task[0x18];
    u8 *background;
    u8 *firstPalette;
    u8 *secondPalette;
};
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void *ScriptProc_Start(u16, u16);
void Bg_SetPriority(u32, s32);
u8 *sub_08216E54(u16);
void PaletteScreen_LoadResources(struct PaletteScreen *p)
{
    u8 *resource;
    u8 *palette;
    s32 layers[2];
    resource = sub_08216E54(0x4ff);
    p->background = resource;
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(1, 0, resource, 0, 0, 2, layers);
    Bg_SetPriority(2, 2);
    Bg_SetPriority(3, 3);
    palette = (u8 *)ScriptProc_Start(0x92b3, 0x663d) + 0x14;
    p->firstPalette = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x4000080);
    p->secondPalette = (u8 *)ScriptProc_Start(0x92b3, 0x673d) + 0x14;
}
