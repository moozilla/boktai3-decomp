#define Player_GetSmoothedDirection sub_08141BAC
#include "global.h"
#include "player.h"
s32 sub_08141BAC(struct Player *p)
{
    s32 result;
    s32 direction;
    s32 clockwise;
    s32 counterclockwise;
    s32 i;
    s16 *current;
    s16 *history;
    s32 previous;
    direction = p->directionHistory[0];
    if (!(direction & 1)) {
        clockwise = (direction + 1) & 7;
        counterclockwise = (direction + 7) & 7;
        i = 1;
        current = &p->directionHistory[0];
        history = &p->directionHistory[1];
        do {
            result = *history;
            previous = result;
            if (result != -1) {
                if (result == clockwise || result == counterclockwise)
                    goto done;
                if (previous != *current)
                    break;
            }
            history++;
            i++;
        } while (i <= 4);
    }
    result = direction;
done:
    return result;
}
