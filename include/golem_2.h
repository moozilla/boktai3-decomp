#ifndef GUARD_GOLEM_2_H
#define GUARD_GOLEM_2_H
#include "scene_actor_reset.h"

/* Private state block the golem actor keeps at actor offset 0x3D0 (context).
 * Field names are offsets until a consumer establishes a role. */
struct GolemState {
    u8 unknown0[0x626];
    u16 countdown626;
    u8 unknown628[0x6AC - 0x628];
    u16 shakeStrength6AC;
    u16 shakeTimer6AE;
};

#endif
