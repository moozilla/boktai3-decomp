#include "global.h"
#include "render.h"
#define gDisplayControlShadow gDispCntShadow
#define gPaletteShadow        gUnk_03004FC0
struct PaletteScreen {
    u8 task[0x18];
    u8 *background;
    u8 *palette;
};
extern u16 gUnk_03004FC0[];
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void *ScriptProc_Start(u16, u16);
void Bg_SetPriority(u32, s32);
u8 *sub_08216E54(u16);
void PaletteScreen_InitDisplayResources(struct PaletteScreen *p)
{
    u8 *resource;
    u8 *palette;
    s32 layers[2];
    {
        u16 *control = &gDispCntShadow;
        u32 mask = 0xfffff0ff;
        u16 value = *control;
        mask &= value;
        {
            u16 enabled = 0xa00;
            *control = mask | enabled;
        }
    }
    resource = sub_08216E54(0x98e2);
    p->background = resource;
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(4, 0, resource, 0, 0, 2, layers);
    Bg_SetPriority(1, 2);
    Bg_SetPriority(3, 3);
    palette = (u8 *)ScriptProc_Start(0x92b3, 0x3cca) + 0x14;
    p->palette = palette;
    CpuSet(palette, gUnk_03004FC0, 0x4000080);
}
