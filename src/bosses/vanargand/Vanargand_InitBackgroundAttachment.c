#include "global.h"
#include "engine/video/render.h"
#include "bosses/vanargand/screen_palette_transition_layout.h"
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
u8 *sub_08216E54(u16);
void ScreenPalette_ProjectPosition(s32, void *, s32, s32);
void Vanargand_InitBackgroundAttachment(u8 *owner, struct ScreenPaletteTransition *part,
                                        void *coordinates)
{
    s32 layer;
    void *resource = sub_08216E54(0xa96a);
    part->resource = resource;
    layer = 2;
    Bg_SetupLayers(3, 0, resource, 0, 0, 1, &layer);
    part->coordinates = coordinates;
    ZeroFill8(part->offset);
    part->timer = 0;
    part->enabled = 0;
    part->state = 0;
    {
        u16 *control = &gDispCntShadow;
        s32 mask = ~0x200;
        *control = mask & *control;
    }
    part->palettes = owner + 0x918;
    ScreenPalette_ProjectPosition(1, part->coordinates, 136, -70);
}
