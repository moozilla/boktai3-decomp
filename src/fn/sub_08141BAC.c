#include "global.h"
#include "player.h"

s32 Player_GetSmoothedDirection(struct Player *arg)
{
    struct Player *p = arg;
    s32 direction = (s16)p->directionHistory[0];
    s32 next;
    s32 previous, i;
    const s16 *base;
    const s16 *history;
    s32 missing;
    if (direction & 1)
        return direction;
    next = (direction + 1) & 7;
    previous = (direction + 7) & 7;
    i = 1;
    base = (const s16 *)&p->directionHistory[0];
    missing = -1;
    history = (const s16 *)&p->directionHistory[1];
    do {
        s32 sample = *history;
        s16 result;
        if (sample != missing) {
            result = *history;
            if (result == next || result == previous)
                return result;
            if (sample != *base)
                return direction;
        }
        history++;
    } while (++i <= 4);
    return direction;
}
