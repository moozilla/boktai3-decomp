#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
#define ThresholdTrack_GetBoundary Bestiary_GetCategoryFirstEntry
s32 Bestiary_GetCategoryFirstEntry(u32);
s32 ThresholdTrack_ClassifyPosition(s32 position)
{
    if (position < Bestiary_GetCategoryFirstEntry(3))
        return 0;
    if (position < Bestiary_GetCategoryFirstEntry(5))
        return 1;
    if (position < Bestiary_GetCategoryFirstEntry(11))
        return 2;
    if (position < Bestiary_GetCategoryFirstEntry(13))
        return 3;
    return 4;
}
