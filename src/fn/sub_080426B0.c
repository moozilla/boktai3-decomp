#include "display_animation.h"
s32 DisplayAnimation_SetPosition(struct DisplayAnimation *display,
                                 const struct DisplayPosition *position)
{
    if (!display->kind)
        return -1;
    *display->position = *position;
    /* No return on success: the ROM leaves the incoming display pointer in r0. */
}
