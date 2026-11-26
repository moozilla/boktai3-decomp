#ifndef GUARD_HIT_SPARK_RANDOM_H
#define GUARD_HIT_SPARK_RANDOM_H

#include "global.h"

/* The monster hit reactions' spark bursts draw three samples per spark from the random table,
 * holding the cursor in a register and storing it back once. The table address lives in a
 * variable the loop reuses for the spawn call's argument, so loop.c keeps its load in the body. */
// confidence=low: helper names are tentative.
static inline u32 HitSpark_NextIndex(u32 index, u32 limit)
{
    return (index + 1) & limit;
}

static inline u32 HitSpark_Sample(u32 table, u32 index)
{
    return ((const u16 *)table)[index];
}

#endif
