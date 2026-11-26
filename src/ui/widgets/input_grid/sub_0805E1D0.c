#include "gba/io_reg.h"
#include "ui/widgets/scene_input.h"
#include "engine/video/palette_pulse.h"
s32 SceneInput_TickChoice(struct AnimSprite *widget, void *resource, u8 *selection, u8 *blink)
{
    u32 change;
    if (gPadState[1] & A_BUTTON) {
        widget->flags |= 1;
        return *selection;
    }
    if (gPadState[1] & B_BUTTON) {
        widget->flags |= 1;
        return 0;
    }
    change = 0;
    if (*selection) {
        if ((u16)(gPadState[1] & DPAD_RIGHT))
            change = 1;
    } else {
        if (gPadState[1] & DPAD_LEFT)
            change = 1;
    }
    if (change) {
        Sound_PlaySong(0xDC);
        *selection = 1 - *selection;
        if (*selection)
            Anim_SelectWidgetGlyph(widget, resource, 0x4B);
        else
            Anim_SelectWidgetGlyph(widget, resource, 0x4C);
    }
    if (blink)
        Palette_UpdateHighlightPulse(widget->palette + 12, blink);
    return -1;
}
