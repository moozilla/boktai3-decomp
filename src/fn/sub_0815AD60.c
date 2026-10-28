// COMPILER: old_agbcc
#include "global.h"
#include "player.h"
/* Player_SetField460FromScript: sets player (slot 0) actor's byte +0x460 to 1; distinct flag from the 45d/45f pair, field purpose unclear so named by offset */
#define Player_SetField460FromScript sub_0815AD60

s32 sub_0815AD60(void)
{
    (*(u8 **)gPlayers)[0x460] = 1;
    return 0;
}
