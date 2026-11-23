#define gSpriteTileBase gPaletteBankData
#include "global.h"
#include "render.h"

/* HUD interpretation and descriptive names are tentative. */
struct HudIcon {
    u8 pad0[0x3a];
    u16 tile;
    u8 pad3c[0xc];
    void *tiles;
    u8 pad4c[0x14];
};
struct Hud {
    u8 pad0[0x64];
    u8 resource[0xAA4];
    struct HudIcon icons[3];
};
struct HudIconOffset {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void Hud_InitializeThreeIcons(struct Hud *hud)
{
    struct HudIconOffset offset;
    s32 i;
    offset.y = 8;
    offset.z = 0;
    for (i = 0; i < 3; i++) {
        offset.x = (i + 14) * 8;
        Widget_InitLinked(&hud->icons[i], hud->resource, 0x29, 0x10031, 0, 0, 60, &offset);
        hud->icons[i].tile = 0;
        hud->icons[i].tiles = gPaletteBankData + (hud->icons[i].tile << 5);
    }
}
