#ifndef GUARD_PLAYER_SLOT_STATUS_H
#define GUARD_PLAYER_SLOT_STATUS_H

#include "player.h"

static inline u32 PlayerSlot_ReadStatus(s32 index, u32 mask)
{
    return gPlayerSlotStatus[index] & mask;
}

#endif
