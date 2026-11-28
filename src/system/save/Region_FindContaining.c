#include "global.h"
#include "engine/ram.h"
#include "system/save/region_list.h"

u8 Terrain_IsPositionInBox(u16 region, void *position); /* position inside region? */

/* The first listed region containing position, or -1; callers keep it as
   the object's region. */
s32 Region_FindContaining(void *position)
{
    struct RegionList *list = gRegionList;
    s32 i;
    s32 *region;
    if (list == 0)
        return -1;
    for (i = 0, region = list->regions; i < 16; region++, i++) {
        if (*region <= 0)
            return -1;
        if (Terrain_IsPositionInBox(*region, position))
            return *region;
    }
    return -1;
}
