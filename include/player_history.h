#ifndef GUARD_PLAYER_HISTORY_H
#define GUARD_PLAYER_HISTORY_H

#include "player.h"

static inline s32 Player_ReadHistoryDirection(struct Player *player)
{
    const s16 *history;
    s32 direction = (s16)player->directionHistory[1];
    s32 missing;
    s32 next, previous, i;

    if (direction == -1)
        return player->facing;
    if (direction & 1)
        return direction;

    next = (direction + 1) & 7;
    previous = (direction + 7) & 7;
    i = 2;
    missing = -1;
    history = (const s16 *)&player->directionHistory[2];
    do {
        s32 sample = *history;
        if (sample != missing) {
            if (sample == next || sample == previous)
                return sample;
            if (sample != direction)
                return direction;
        }
        history++;
    } while (++i <= 9);
    return player->facing;
}

#endif
