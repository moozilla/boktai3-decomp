#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
u32 ScriptActor_TestFlagBit20FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p != 0 && tst((u32 *)(p + 0x20), 0x100000))
        goto one;
    return 0;
one:
    return 1;
}
