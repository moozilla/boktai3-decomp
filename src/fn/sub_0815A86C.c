#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void ScriptActor_ClearField534FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p) {
        u16 *a = (u16 *)(p + 0x534);
        u32 z;
        z = 0;
        *a = z;
        *(u16 *)(p + 0x538) = z;
        *(u16 *)(p + 0x536) = z;
    }
}
