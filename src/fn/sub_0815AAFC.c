#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void ScriptActor_SetField4A2FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p)
        *(u16 *)(p + 0x4a2) = 1;
}
