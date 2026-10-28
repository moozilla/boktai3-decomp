// COMPILER: old_agbcc
#include "global.h"
#include "player.h"
/* Player_SetField45DField45FFromScript: sets player (slot 0) actor's bytes +0x45d and +0x45f both to 1 (variant of Player_SetField45DFromScript that also sets the adjacent byte) */
#define Player_SetField45DField45FFromScript sub_0815AD3C

s32 sub_0815AD3C(void)
{
    u8 *p = (*(u8 **)gPlayers);
    p[0x45d] = 1;
    p[0x45f] = 1;
    return 0;
}
