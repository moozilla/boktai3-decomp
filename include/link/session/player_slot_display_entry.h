#ifndef GUARD_INCLUDE_LINK_SESSION_PLAYER_SLOT_DISPLAY_ENTRY_H
#define GUARD_INCLUDE_LINK_SESSION_PLAYER_SLOT_DISPLAY_ENTRY_H

#include "global.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_position.h"

struct PlayerSlotDisplaySlot {
    u8 resource[0x1C];
    struct DisplaySequenceSprite sprite; /* confidence=low */
    struct DisplaySequenceState sequence; /* confidence=low */
    const struct ContactPosition *source; /* confidence=low */
    u16 stopOnEnd; /* confidence=low */
    u16 active;
};

typedef char PlayerSlotDisplaySlotSizeCheck[sizeof(struct PlayerSlotDisplaySlot) == 0x60 ? 1 : -1];

#endif
