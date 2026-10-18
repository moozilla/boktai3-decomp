#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
#define ThresholdTrack_GetBoundary Bestiary_GetCategoryFirstEntry
struct ThresholdTrack {
    u8 pad0[0x25];
    u8 interval;
    u8 position;
    u8 pad27[0x21];
    u8 remainder;
};
s32 Bestiary_GetCategoryFirstEntry(u32);
void ThresholdTrack_SelectInterval(struct ThresholdTrack *p)
{
    s32 i;
    for (i = 0; i <= 13; i++) {
        if (p->position < Bestiary_GetCategoryFirstEntry(i)) {
            p->interval = i - 1;
            p->remainder = p->position - Bestiary_GetCategoryFirstEntry(p->interval);
            return;
        }
    }
    p->interval = 13;
    p->remainder = p->position - Bestiary_GetCategoryFirstEntry(13);
}
