#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void Player_ClearStateTimer(u8 *, s32);
void ScriptActor_ClearSlotsFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p) {
        s32 j = 0;
        do {
            Player_ClearStateTimer(p, j);
            j++;
        } while (j <= 1);
    }
}
