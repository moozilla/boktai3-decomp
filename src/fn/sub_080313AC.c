#include "global.h"
#include "ram.h"
/* MessageWindow_CheckExists: Returns zero when the message window singleton exists and minus one otherwise */
#define MessageWindow_CheckExists sub_080313AC

s32 sub_080313AC(void)
{
    if (gMessageWindow == NULL)
        return -1;
    return 0;
}
