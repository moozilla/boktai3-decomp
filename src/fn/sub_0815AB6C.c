#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);

u8 sub_0815AB6C(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor == NULL)
        return 0;
    return actor[0xbf5];
}
