#include "display_animation.h"
#define DisplayAnimation_SetPalette sub_0804275C
s32 DisplayAnimation_SetPalette(struct DisplayAnimation *display, u32 palette, u32 variant,
                                u32 attribute)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1) {
        struct DisplaySequenceSprite *sprite =
            (struct DisplaySequenceSprite *)((u8 *)display + 0x28);
        sprite->palette = palette;
        sprite->paletteVariant = variant;
        sprite->attribute = attribute;
    } else {
        struct DisplayKeyframeState *state = (struct DisplayKeyframeState *)((u8 *)display + 0x2C);
        state->palette = palette;
        state->paletteVariant = variant;
        state->attribute = attribute;
    }
    return 0;
}
