#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Player_ClearStateTimer(u8 *, s32);
void ScriptActor_ClearSlotFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p != 0 && Script_SeekToKeyword(0x73) != 0)
        Player_ClearStateTimer(p, Script_GetValue());
}
