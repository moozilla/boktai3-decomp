#define Registry3F74_InitBackground sub_081BA924
#define Registry3F74_ApplyPaletteStep sub_081BA644
#define gRegistry3F74ObjPalette gUnk_03005020
#define gDisplayControlShadow gUnk_03004BD8
#include "global.h"

struct Registry3F74Object { u8 unknown[0x18]; void *background; u16 *palettes; };
extern u16 gUnk_03004BD8;
extern u16 gUnk_03005020[];
void Bg_SetPriority(u32, s32);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Hud_SetMode(s32);
void *ScriptProc_Start(u16, u16);
/* This caller uses the phase as a word without narrow-return extension. */
u32 SunClock_GetPhase(void);
void sub_082157EC(void);
u8 *sub_08216E54(u16);
void sub_081BA644(struct Registry3F74Object *, u32);
void sub_081BA924(struct Registry3F74Object *p)
{
    u8 *resource;
    s32 layers[2];
    {
        u16 *control = &gUnk_03004BD8;
        u32 mask = ~0xF00;
        u32 flags = *control;
        mask &= flags;
        *control = mask;
    }
    sub_082157EC();
    Hud_SetMode(7);
    {
        u32 flags = 0xC00;
        flags |= gUnk_03004BD8;
        flags |= 0x100;
        gUnk_03004BD8 = flags;
    }
    p->background = sub_08216E54(0xDC87);
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(1, 0, p->background, 0, 0, 2, layers);
    Bg_SetPriority(3, 3);
    Bg_SetPriority(2, 2);
    resource = ScriptProc_Start(0x92B3, 0x771E);
    p->palettes = (u16 *)(resource + 0x14);
    sub_081BA644(p, SunClock_GetPhase());
    CpuSet(resource + 0x74, gUnk_03005020, 0x04000008);
}
