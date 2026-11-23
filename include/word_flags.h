#ifndef GUARD_WORD_FLAGS_H
#define GUARD_WORD_FLAGS_H

#include "global.h"

static inline u32 WordFlags_Test(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u32 Flags_MaskedWord(u32 mask, const u32 *flags)
{
    return *flags & mask;
}

static inline void WordFlags_Set(u32 mask, u32 *flags)
{
    *flags |= mask;
}

/* The halfword form takes a u16 mask: agbcc then loads the mask before the field's address. */
static inline void HalfwordFlags_Set(u16 mask, u16 *flags)
{
    *flags |= mask;
}

#endif
