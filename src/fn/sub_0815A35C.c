#include "global.h"
#include "render.h"
#include "player.h"

struct O0815A35C {
    u8 f0[0xD4];
    u32 fD4;
};

u32 Player_GetCollisionData(void)
{
    if ((*(struct O0815A35C **)gPlayers) == 0)
        return (u32)(gPaletteBankData + 0x540);
    return (*(struct O0815A35C **)gPlayers)->fD4;
}
