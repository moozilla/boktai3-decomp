#include "global.h"
#include "player/player.h"

s32 Player_GetInputDirection(struct Player *);

void sub_082420D4(struct Player *p)
{
    s32 i;
    s32 direction;

    for (i = 9; i > 0; i--)
        p->directionHistory[i] = p->directionHistory[i - 1];
    direction = Player_GetInputDirection(p);
    p->directionHistory[0] = direction;
    if ((s16)direction >= 0) {
        s32 turned = direction;
        turned += (s8)p->catalogSavedOption;
        p->directionHistory[0] = (turned + 7) & 7;
    }
}
