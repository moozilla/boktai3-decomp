#include "projected_display.h"
struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *, s32);
struct ProjectedDisplayFrame {
    u16 frame, unknown2;
};
extern const struct ProjectedDisplayFrame gUnk_08605960[];
s32 sub_0821983C(void *, void *, u32, u32, u32, u32, u32, void *);
s32 ProjectedDisplay_InitializeEntries(struct ProjectedDisplay *display)
{
    struct ProjectedDisplayEntry *entry = display->entries;
    s32 index = 0;
    struct ProjectedDisplayResource *resource = &display->resource;
    s32 result;
    do {
        if (sub_0821983C(entry, resource, 0, 16, 2, 0, 60, &display->screen) >= 0)
            goto initialized;
        result = -1;
        goto done;
    initialized:
        Anim_SetKeyframes(entry, resource, gUnk_08605960[index].frame, 1, 4);
        ProjectedDisplay_PositionEntry(display, index);
        index++;
        entry++;
    } while (index <= 4);
    result = 0;
done:
    return result;
}
