#include "global.h"
extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
u32 ScriptActor_TestFlagBit20FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p != 0 && tst((u32 *)(p + 0x20), 0x100000))
        goto one;
    return 0;
one:
    return 1;
}
