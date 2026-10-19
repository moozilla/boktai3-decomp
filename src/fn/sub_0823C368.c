
#ifdef NONMATCHING
// fuzzy: 91.8%
// foundry draft (worker best 91.7%); what differs is mostly register allocation and instruction order
#define Player_SelectHistoryDirection sub_0823C368
#include "global.h"
#include "player.h"
s32 sub_0823C368(struct Player *s)
{
    s32 i, previous, direction, clockwise, counterclockwise;
    s16 *history = s->directionHistory;
    direction = history[1];
    if (direction != -1) {
        if (direction & 1) {
returnDirection:
            return direction;
        }
        clockwise = (direction + 1) & 7;
        counterclockwise = (direction + 7) & 7;
        for (i = 2; i < 10; i++) {
            previous = history[i];
            if (previous == -1) continue;
            if (previous == clockwise) return previous;
            if (previous == counterclockwise) return previous;
            if (previous != direction) goto returnDirection;
        }
    }
    return s->facing;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823C368);
#endif
