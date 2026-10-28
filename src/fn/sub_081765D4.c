#define gDisplayControlShadow gDispCntShadow
#include "global.h"
#include "background_controller.h"
#include "render.h"
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
void sub_08215AC4(u32, u32);
void Mosaic_ResetParameters(void);
void SceneDialog_ConfigureVariantSlot(struct PauseMenuDisplay *);
void PauseMenu_SaveDisplayState(struct PauseMenuDisplay *s)
{
    u32 flags;
    u32 bgEnable;
    s->active = 1;
    gRenderListIndex = 1;
    flags = gDispCntShadow;
    s->savedDisplay = flags;
    bgEnable = 0x100;
    flags &= ~bgEnable;
    flags &= ~0x200;
    flags &= ~0x400;
    flags &= ~0x800;
    gDispCntShadow = flags;
    CpuSet(Video_GetBackgroundMap(1), s->savedMap, 0x04000200);
    s->savedBackground = gBgRecords[1];
    s->savedBlend = *(vu16 *)0x04000050;
    s->savedBgControl = *(vu16 *)0x0400000A;
    *(vu16 *)0x04000050 = 0;
    gFadeMultiplier = 0x40;
    sub_08215AC4(1, 2);
    Bg_SetPriority(1, 1);
    s->saved521C = gMosaicHalfword;
    s->saved5234 = gMosaicParam;
    s->saved5230 = gSpriteMosaicAttr;
    s->saved5218 = gMosaicByte;
    Mosaic_ResetParameters();
    gDispCntShadow |= bgEnable;
    s->state = 0;
    SceneDialog_ConfigureVariantSlot(s);
    s->selection = 0xFF;
}
