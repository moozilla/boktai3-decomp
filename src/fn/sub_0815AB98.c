#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);

void Player_EnterKindFour(u8 *);
void sub_0815AB98(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL)
        Player_EnterKindFour(actor);
}
