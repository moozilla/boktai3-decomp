// MATCHING_CONCESSION: register-pinning
#include "scene_input.h"
s32 SceneInput_TickChoice(struct SceneInputWidget *widget, void *resource, u8 *selection, u8 *blink)
{
    u32 keys = gPadState[1];
    u32 change;
    u32 confirm = 1;
    u32 accepted;
    accepted = confirm;
    asm("" : "+r"(accepted));
    accepted &= keys;
    if (accepted) {
        widget->flags |= confirm;
        return *selection;
    }
    if (keys & 2) {
        widget->flags |= confirm;
        return 0;
    }
    change = 0;
    if (*selection) {
        u32 pressed = 0x10;
        pressed &= keys;
        /* Normalize the narrowed key mask to 0 or 1. */
        change = (u32)(-(s32)(u16)pressed) >> 31;
    } else {
        u32 pressed = 0x20;
        pressed &= keys;
        if (pressed)
            change = 1;
    }
    if (change) {
        u32 value;
        Sound_PlaySong(0xDC);
        value = 1 - *selection;
        *selection = value;
        if ((u8)value)
            Anim_SelectWidgetGlyph(widget, resource, 0x4B);
        else
            Anim_SelectWidgetGlyph(widget, resource, 0x4C);
    }
    if (blink)
        Palette_UpdateHighlightPulse(widget->palette + 12, blink);
    return -1;
}
