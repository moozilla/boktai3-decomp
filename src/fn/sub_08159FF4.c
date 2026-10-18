#include "global.h"
#include "player.h"

s32 Player_RequestTimedReset(struct Player *p, u32 timer)
{
    if (p->kind != 1)
        return 0;
    if (p->positionResetTimer && p->tetherMode == 0x15 && p->positionResetCounter == 1) {
        p->positionResetRequested = 1;
        return 0;
    }
    p->positionResetTimer = timer;
    p->positionResetRequested = 1;
    return 1;
}
