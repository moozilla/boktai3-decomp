#define StatGauge_ResetBackground sub_081BC354
#define gBackgroundPalette gUnk_03005160
#include "global.h"
extern u16 gUnk_03005160[];
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
struct StatGaugePreview {
    u8 unknown0[0x8e0];
    void *map;
    void *palette;
};
void sub_081BC354(struct StatGaugePreview *p)
{
    u32 zero = 0;
    Bg_SetupLayers(0, 0, p->map, 0, 0, 1, &zero);
    Bg_SetLayerMap(0, p->map, 0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    CpuSet(p->palette, gBackgroundPalette, 0x04000018);
}
