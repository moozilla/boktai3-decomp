#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetField534FromScript(void)
{
    u8 *actor;
    u8 *a;
    s32 i = Script_GetActorIndex();
    a = gPlayers[i];
    actor = a;
    if (a == 0)
        return -1;
    if (*(u16 *)(actor + 0x538) == 0)
        return -1;
    return *(u16 *)(actor + 0x534);
}
