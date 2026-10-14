// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#define DisplayAnimation_SetPosition sub_080426B0
s32 DisplayAnimation_SetPosition(struct DisplayAnimation *display,
                                 const struct DisplayPosition *position)
{
    register s32 result asm("r0") = (s32)display;
    const struct DisplayPosition *source = position;
    if (!display->kind) {
        result = -1;
        goto done;
    }
    *display->position = *source;
done:
    return result; /* observed success result retains the incoming pointer */
}
