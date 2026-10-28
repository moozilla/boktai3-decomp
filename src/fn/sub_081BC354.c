#include "global.h"
#include "render.h"
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
struct StatGaugePreview {
    u8 unknown0[0x8e0];
    void *map;
    void *palette;
};
void StatGauge_ResetBackground(struct StatGaugePreview *p)
{
    u32 zero = 0;
    Bg_SetupLayers(0, 0, p->map, 0, 0, 1, &zero);
    Bg_SetLayerMap(0, p->map, 0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    CpuSet(p->palette, gBgPaletteBank13, 0x04000018);
}
