#ifndef GUARD_WORD_FLAGS_H
#define GUARD_WORD_FLAGS_H

#include "gba/types.h"

static inline u32 Flags_MaskedWord(u32 mask, const u32 *flags)
{
    return *flags & mask;
}

#endif
