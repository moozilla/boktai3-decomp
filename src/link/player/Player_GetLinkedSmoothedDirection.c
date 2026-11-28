#include "global.h"
#include "player/player.h"

// Tentative: linked action handlers smooth the recent direction history.
s32 Player_GetLinkedSmoothedDirection(struct Player *p)
{
    s32 direction = (s16)p->directionHistory[0];
    s32 next, previous, i;

    if (direction & 1)
        return direction;
    next = (direction + 1) & 7;
    previous = (direction + 7) & 7;
    for (i = 1; i <= 4; i++) {
        if ((s16)p->directionHistory[i] != -1) {
            if ((s16)p->directionHistory[i] == next || (s16)p->directionHistory[i] == previous)
                return (s16)p->directionHistory[i];
            if ((s16)p->directionHistory[i] != (s16)p->directionHistory[0])
                return direction;
        }
    }
    return direction;
}
