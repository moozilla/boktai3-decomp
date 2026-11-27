#ifndef GUARD_INCLUDE_RECOVERED_PACKED_CENTIPEDE_HALFWORDS_LAYOUT_H
#define GUARD_INCLUDE_RECOVERED_PACKED_CENTIPEDE_HALFWORDS_LAYOUT_H

#include "global.h"

struct PackedCentipedeHalfwords {
    u32 lo : 16;
    u32 hi : 16;
};

typedef char PackedCentipedeHalfwordsSizeCheck[sizeof(struct PackedCentipedeHalfwords) == 0x4 ? 1 : -1];

#endif
