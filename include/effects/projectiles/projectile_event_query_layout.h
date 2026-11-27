#ifndef GUARD_INCLUDE_EFFECTS_PROJECTILES_PROJECTILE_PROJECTILE_EVENT_QUERY_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_PROJECTILES_PROJECTILE_PROJECTILE_EVENT_QUERY_LAYOUT_H

#include "global.h"

struct ProjectileEventQuery {
    u8 f[0x10];
    u16 v;
    u8 g[0x1C - 0x12];
} __attribute__((aligned(4)));

typedef char ProjectileEventQuerySizeCheck[sizeof(struct ProjectileEventQuery) == 0x1c ? 1 : -1];

#endif
