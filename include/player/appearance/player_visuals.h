#ifndef GUARD_PLAYER_APPEARANCE_PLAYER_VISUALS_H
#define GUARD_PLAYER_APPEARANCE_PLAYER_VISUALS_H
#include "engine/engine.h"
#include "player/player.h"
#include "ui/widgets/scene_input_compare.h"
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
