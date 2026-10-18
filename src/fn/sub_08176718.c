#define PauseMenu_RestoreDisplayState sub_08176718
#define gDisplayControlShadow gUnk_03004BD8
#include "global.h"
#include "background_controller.h"
struct PauseMenuDisplay {
    u8 unknown0[0x18];
    u32 resource;
    u8 unknown1C[0x10];
    u32 parameter;
    u8 unknown30[0x1B0];
    struct BackgroundLayer savedBackground;
    u8 savedMap[0x800];
    u16 savedBlend;
    u8 unknownA12[2];
    u16 savedBgControl;
    u8 unknownA16[0x46];
    u16 active;
    u8 unknownA5E[0x3C];
    u8 transition0, transition1;
    u8 unknownA9C[0x3804];
    u8 state;
    u8 unknown42A1[0x137];
    u16 savedDisplay, saved521C, saved5234;
    u8 saved5230, saved5218;
    u8 unknown43E0[0x4CB];
    u8 selection;
};
extern u32 gUnk_03004290;
extern u16 gUnk_0300521C, gUnk_03005234;
extern u8 gUnk_03005230, gUnk_03005218;
void sub_08215AC4(u32, u32);
void PauseMenu_HideAllSprites(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_RequestHide(void);
void PauseMenu_CloseTextWindows(void *);
void Hud_SetupBackgroundLayer(void *, u32, s32, s32, u32);
void sub_08167BDC(void *);
void sub_08176718(struct PauseMenuDisplay *s)
{
    u32 bgEnable;
    u32 flags;
    void *map;
    s->active = 0;
    gUnk_03004290 = 0;
    bgEnable = 0x100;
    flags = gUnk_03004BD8;
    flags &= ~bgEnable;
    flags &= ~0x200;
    gUnk_03004BD8 = flags;
    PauseMenu_HideAllSprites(s);
    sub_08215AC4(1, 0);
    map = Video_GetBackgroundMap(1);
    CpuSet(s->savedMap, map, 0x04000200);
    gUnk_03004C30[1] = s->savedBackground;
    *(vu16 *)0x0400000A = s->savedBgControl;
    gUnk_0300521C = 1;
    gUnk_03005234 = s->saved5234;
    gUnk_03005230 = s->saved5230;
    gUnk_03005218 = s->saved5218;
    MessageWindow_Clear();
    ScrollingDialogue_RequestHide();
    PauseMenu_CloseTextWindows(s);
    gUnk_03004BD8 |= bgEnable;
    Hud_SetupBackgroundLayer(s, s->resource, 0, 0, s->parameter);
    s->transition0 = 0;
    s->transition1 = 0;
    sub_08167BDC(s);
}
