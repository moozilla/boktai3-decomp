#ifndef GUARD_PLAYER_TIMED_CUE_H
#define GUARD_PLAYER_TIMED_CUE_H

#include "ram.h"

static inline void Player_SetTimedCue(u16 kind, u16 duration)
{
    gPlayerActionPending = kind;
    gPlayerCueTimer = duration;
}

#endif
