#include "global.h"
#include "player.h"

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 SceneActorControl_BindPrimaryPlayer(u8 *p)
{
    *(u8 **)(p + 0xdc) = (u8 *)gPlayers[0] + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
    return 0;
}
