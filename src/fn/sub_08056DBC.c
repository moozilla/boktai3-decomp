#include "shake_icon_display.h"
s32 ShakeIconDisplay_Tick(struct ShakeIconDisplay *display)
{
    u16 *frame;
    u32 value;
    display->update(display);
    frame = &display->stateFrame;
    value = *frame + 1;
    *frame = value;
    if ((u16)value >= display->lifetime) {
        if (display->completionScriptId)
            Script_Call(display->completionScriptId, 0);
        Task_Kill(display);
    }
    /* Preserve the original incidental nonvoid fallthrough; the task ignores it. */
}
