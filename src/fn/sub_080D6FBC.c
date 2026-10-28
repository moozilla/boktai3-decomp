#include "global.h"
#include "player.h"

bool32 sub_080D6FBC(void)
{
    u8 *player = gPlayers[0];
    u32 mask = 8;
    if (*(u32 *)(player + 0x20) & mask)
        return TRUE;
    return FALSE;
}
