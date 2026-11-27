#include "global.h"
struct Storage {
    u8 pad0[0x172c];
    void *background;
};
void Bg_SetLayerMap(u32, void *, u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void *ScriptProc_Start(u16, u16);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void Storage_InitBackground(struct Storage *p)
{
    void *background;
    s32 zero;
    background = ScriptProc_Start(0xc091, 0xa597);
    p->background = background;
    zero = 0;
    Bg_SetupLayers(0, 0, background, 0, 0, 1, &zero);
    Bg_SetLayerMap(0, p->background, 0x10);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
}
