#include "global.h"

extern const u32 gUnk_086056D0[];
u32 ScrollingDialogue_SetScript(const u32 *);
u32 ScrollingDialogue_ClearScript(void)
{
    return ScrollingDialogue_SetScript(gUnk_086056D0);
}
