#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);

void sub_0815AB24(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL)
        actor[0x340] = 1;
}
