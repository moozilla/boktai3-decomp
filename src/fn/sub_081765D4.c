#define gDisplayControlShadow gUnk_03004BD8
#include "global.h"
#include "background_controller.h"
struct PauseMenuDisplay {
    u8 unknown0[0x1E0];
    struct BackgroundLayer savedBackground;
    u8 savedMap[0x800];
    u16 savedBlend;
    u8 unknownA12[2];
    u16 savedBgControl;
    u8 unknownA16[0x46];
    u16 active;
    u8 unknownA5E[0x3842];
    u8 state;
    u8 unknown42A1[0x137];
    u16 savedDisplay, saved521C, saved5234;
    u8 saved5230, saved5218;
    u8 unknown43E0[0x4CB];
    u8 selection;
};
extern u32 gUnk_03004290, gUnk_030051D8;
extern u16 gUnk_0300521C, gUnk_03005234;
extern u8 gUnk_03005230, gUnk_03005218;
void sub_08215AC4(u32, u32);
void Mosaic_ResetParameters(void);
void SceneDialog_ConfigureVariantSlot(struct PauseMenuDisplay *);
void PauseMenu_SaveDisplayState(struct PauseMenuDisplay *s)
{
    u32 flags;
    u32 bgEnable;
    s->active = 1;
    gUnk_03004290 = 1;
    flags = gUnk_03004BD8;
    s->savedDisplay = flags;
    bgEnable = 0x100;
    flags &= ~bgEnable;
    flags &= ~0x200;
    flags &= ~0x400;
    flags &= ~0x800;
    gUnk_03004BD8 = flags;
    CpuSet(Video_GetBackgroundMap(1), s->savedMap, 0x04000200);
    s->savedBackground = gUnk_03004C30[1];
    s->savedBlend = *(vu16 *)0x04000050;
    s->savedBgControl = *(vu16 *)0x0400000A;
    *(vu16 *)0x04000050 = 0;
    gUnk_030051D8 = 0x40;
    sub_08215AC4(1, 2);
    Bg_SetPriority(1, 1);
    s->saved521C = gUnk_0300521C;
    s->saved5234 = gUnk_03005234;
    s->saved5230 = gUnk_03005230;
    s->saved5218 = gUnk_03005218;
    Mosaic_ResetParameters();
    gUnk_03004BD8 |= bgEnable;
    s->state = 0;
    SceneDialog_ConfigureVariantSlot(s);
    s->selection = 0xFF;
}
