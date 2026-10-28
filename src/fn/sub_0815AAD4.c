#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void sub_0815AAD4(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL)
        *(u16 *)(actor + 0x52e) = 1;
}