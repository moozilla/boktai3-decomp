#include "global.h"
#include "player/player.h"

u32 Player_CanUseAction(struct Player *p, s32 action)
{
    if (p->actionMode <= 1 || p->actionMode == 6) {
        if (action == 0 || action == 3 || action == 6 || (action == 4 && p->actionFourEnabled))
            return 1;
        return 0;
    }
    if (action == 0 || action == 4)
        return 1;
    return 0;
}
