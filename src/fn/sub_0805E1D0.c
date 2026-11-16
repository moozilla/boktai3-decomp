#include "scene_input.h"
s32 SceneInput_TickChoice(struct SceneInputWidget *widget, void *resource, u8 *selection, u8 *blink)
{
    void *savedResource = resource;
    u8 *savedSelection = selection;
    u8 *savedBlink = blink;
    u32 change;
    u32 confirm;
    u32 accepted;
    resource = (void *)(u32)gPadState[1];
    confirm = 1 + ((u32)resource >> 16);
    accepted = confirm;
    accepted &= (u32)resource;
    if (accepted) {
        widget->flags |= confirm;
        return *savedSelection;
    }
    if ((u32)resource & 2) {
        widget->flags |= confirm;
        return 0;
    }
    change = 0;
    if (*savedSelection) {
        u32 pressed = 0x10;
        pressed &= (u32)resource;
        /* Normalize the narrowed key mask to 0 or 1. */
        change = (u32)(-(s32)(u16)pressed) >> 31;
    } else {
        u32 pressed = 0x20;
        pressed &= (u32)resource;
        if (pressed)
            change = 1;
    }
    if (change) {
        u32 value;
        Sound_PlaySong(0xDC);
        value = 1 - *savedSelection;
        *savedSelection = value;
        if ((u8)value)
            Anim_SelectWidgetGlyph(widget, savedResource, 0x4B);
        else
            Anim_SelectWidgetGlyph(widget, savedResource, 0x4C);
    }
    if (savedBlink)
        Palette_UpdateHighlightPulse(widget->palette + 12, savedBlink);
    return -1;
}
