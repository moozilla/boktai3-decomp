#include "global.h"
/* MessageWindow_CheckExists: Returns zero when the message window singleton exists and minus one otherwise */
#define MessageWindow_CheckExists sub_080313AC

extern void *gUnk_020000DC;

s32 sub_080313AC(void)
{
    if (gUnk_020000DC == NULL)
        return -1;
    return 0;
}
