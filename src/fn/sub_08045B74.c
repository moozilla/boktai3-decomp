#define ProjectedDisplay_RemoveEntries sub_08045B74
#include "projected_display.h"
void ProjectedDisplay_RemoveEntries(struct ProjectedDisplay *display)
{
    s32 remaining;
    struct ProjectedDisplayEntry *entry = display->entries;
    for (remaining = 4; remaining >= 0; remaining--, entry++)
        Anim_Remove(entry);
}
