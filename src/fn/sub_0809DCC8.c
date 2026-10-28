#include "global.h"
#include "player.h"

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Lun_OnPlayerTargetInit(u8 *p)
{
    *(u8 **)(p + 0xdc) = (*(u8 **)gPlayers) + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
    return 0;
}
