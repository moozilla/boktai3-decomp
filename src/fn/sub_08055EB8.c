#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.0%
#include "stage_icon_display.h"
s32 StageIconDisplay_SelectUpdate(struct StageIconDisplay *display, s32 variant)
{
    if (variant == 0)
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickReveal);
    else if (variant == 1)
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickDelay);
    else
        StageIconDisplay_SetUpdate(display, StageIconDisplay_TickReveal);
    /* Draft normalizes the ROM's incidental positive return-address word to
     * success. Both observed creators test only its sign. Retain ASM for exact
     * runtime behavior; the original bits have no established game meaning. */
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", StageIconDisplay_SelectUpdate);
#endif
