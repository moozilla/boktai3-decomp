#define Player_SelectHistoryDirection sub_08141A1C
#include "global.h"
#include "player.h"
s32 sub_08141A1C(struct Player *p)
{
    s32 current = p->directionHistory[1];
    s32 next, previous, i;
    if (current != -1) {
        if (current & 1) {
        useCurrent:
            return current;
        }
        next = (current + 1) & 7;
        previous = (current + 7) & 7;
        for (i = 2; i <= 9; i++) {
            s32 direction = p->directionHistory[i];
            if (direction != -1) {
                if (direction == next)
                    return direction;
                if (direction == previous)
                    return direction;
                if (direction != current)
                    goto useCurrent;
            }
        }
    }
    return p->facing;
}
