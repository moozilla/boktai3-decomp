#include "projected_display.h"
struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *, s32);
void ProjectedDisplay_UpdateEntries(struct ProjectedDisplay *display)
{
    struct ProjectedDisplayEntry *entry = display->entries;
    s32 index = 0;
    do {
        Anim_Step(entry, &display->resource);
        ProjectedDisplay_PositionEntry(display, index);
        index++;
        entry++;
    } while (index <= 4);
}
