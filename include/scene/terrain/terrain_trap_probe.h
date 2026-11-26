#ifndef GUARD_TERRAIN_TRAP_PROBE_H
#define GUARD_TERRAIN_TRAP_PROBE_H

#include "scene/collision/contact_position.h"

struct TrapProbe {
    u16 kind;
    u8 unknown2, count;
    struct ContactPosition *position;
    u32 serials[10];
};

typedef char TrapProbeSizeCheck[sizeof(struct TrapProbe) == 0x30 ? 1 : -1];

#endif
