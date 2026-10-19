#ifndef GUARD_PLAYER_3_H
#define GUARD_PLAYER_3_H
#include "engine.h"
#include "player.h"
#include "scene_input_compare.h"
/* Modifier callback input, distinct from the owning player (0xB0 selects equipment). */
struct PlayerModifierInput {
    u8 unknown0[0xB0];
    u8 equipment;
};
struct PlayerModifierTarget {
    u8 unknown0[0x38];
    u32 flags;
};
#endif
