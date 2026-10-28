// COMPILER: old_agbcc
#include "global.h"
#include "player.h"
/* Player_SetField45DFromScript: sets player (slot 0) actor's byte +0x45d to 1; no script/index lookup, field purpose unclear so named by offset */
#define Player_SetField45DFromScript sub_0815AD24

s32 sub_0815AD24(void)
{
    (*(u8 **)gPlayers)[0x45d] = 1;
    return 0;
}
