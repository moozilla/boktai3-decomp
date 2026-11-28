#ifndef GUARD_REGION_LIST_H
#define GUARD_REGION_LIST_H

#include "global.h"
#include "engine/task.h"

/* Task behind gRegionList: up to 16 region ids (positive; the list ends at the first id <= 0). */
struct RegionList {
    struct Task task;
    s32 regions[16]; /* 0x18 */
};

typedef char RegionListSizeCheck[sizeof(struct RegionList) == 0x58 ? 1 : -1];

#endif
