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

static inline u8 WordFlags_Has(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}

static inline void WordFlags_And(u32 mask, u32 *flags)
{
    *flags = mask & *flags;
}

static inline void HalfwordFlags_Clear(u16 *flags, u32 mask)
{
    u32 keep = ~mask;
    *flags = keep & *flags;
}

static inline void HalfwordFlags_And(u16 *flags, s32 mask)
{
    *flags &= mask;
}

#endif
