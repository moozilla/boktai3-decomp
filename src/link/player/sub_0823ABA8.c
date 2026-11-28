#include "global.h"
#include "player/player.h"

bool32 sub_0823ABA8(struct Player *p, s32 amount)
{
    if (p->currentStatB < amount)
        return FALSE;
    return TRUE;
}
