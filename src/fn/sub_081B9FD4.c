#define gDisplayControlShadow gDispCntShadow
#include "global.h"
#include "engine.h"
#include "render.h"

struct Registry2388PaletteController {
    u8 pad0[0x1c];
    u8 *palettes;
};
extern u16 gDisplayControlShadow;
void sub_082157EC(void);
void Hud_SetMode(s32);
u8 *sub_08216E54(u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Bg_SetPriority(u32, s32);

void Registry2388_InitializeBackground(struct Registry2388PaletteController *p)
{
    u8 *background;
    s32 config[2];
    u16 *display = &gDisplayControlShadow;
    s32 clearMask = ~0xf00;
    *display = clearMask & *display;
    sub_082157EC();
    Hud_SetMode(7);
    {
        u32 enabledLayers = 0xd00;
        *display = enabledLayers | *display;
    }
    background = sub_08216E54(0xdcf7);
    config[0] = 1;
    config[1] = 0;
    Bg_SetupLayers(1, 0, background, 0, 0, 2, config);
    Bg_SetPriority(3, 3);
    Bg_SetPriority(2, 2);
    p->palettes = (u8 *)ScriptProc_Start(0x92b3, 0x7e1e) + 0x14;
    CpuSet(p->palettes, gBgPaletteBuffer, 0x04000080);
}
