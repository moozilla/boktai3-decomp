#include "global.h"
#include "player.h"

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 TentacleEnemy_InitPlayerReference(u8 *p)
{
    *(u8 **)(p + 0xdc) = (*(u8 **)gPlayers) + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
    return 0;
}
