#include "global.h"
#include "player.h"
u32 Player_GetTetherMode(struct Player *s)
{
    s8 *action = (s8 *)&s->thresholdMode;
    if (*action < 0)
        return s->tetherMode;
    if (((u8 *)action)[1] == 0)
        return s->tetherMode;
    if (s->tetherMode > 1)
        return s->tetherMode;
    return s->tetherMode;
}
