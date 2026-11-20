#ifndef GUARD_WORD_FLAGS_H
#define GUARD_WORD_FLAGS_H

#include "global.h"

static inline u32 WordFlags_Test(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline void WordFlags_Set(u32 *flags, u32 mask)
{
    *flags |= mask;
}

#endif
