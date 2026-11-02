#include "background_controller.h"
struct SceneLoadScreen {
    u8 unknown0[0x18];
    void *map;
};
struct SceneLoadPalette {
    u8 header[0x14];
    u16 colors[256];
};
void *SceneTransition_StartSunScript(void);
void sub_081C4708(struct SceneLoadScreen *screen)
{
    void *resource;
    s32 layer;
    u16 tile;
    u16 firstTile;
    struct SceneLoadPalette *palette;
    {
        u16 *control = &gDispCntShadow;
        u32 mask = 0xFFFFF0FF;
        u16 flags = *control;
        mask &= flags;
        {
            u16 enabled = 0x800;
            *control = mask | enabled;
        }
    }
    resource = sub_08216E54(0xB65E);
    screen->map = resource;
    layer = 0;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &layer);
    Bg_SetPriority(3, 3);
    resource = ScriptProc_Start(0xCEEF, 0x8E32);
    {
        struct BackgroundResource *tiles = resource;
        tile = 0xD759;
        firstTile = tiles->size;
    }
    sub_08215D40(2, resource, 1, &tile, firstTile);
    palette = SceneTransition_StartSunScript();
    CpuSet(palette->colors, gBgPaletteBuffer, 0x04000080);
}
