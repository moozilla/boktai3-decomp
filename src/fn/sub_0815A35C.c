#include "global.h"
#include "render.h"
#include "player.h"

u32 Player_GetCollisionData(void)
{
    if (gPlayers[0] == 0)
        return (u32)(gPaletteBankData + 0x540);
    return gPlayers[0]->actionVariantResource;
}
