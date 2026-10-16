#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.0%
// Open: the original copies the loaded actor pointer (ldr r0; adds r1, r0).
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
/* Script: returns the halfword at +0x534 of the scripted actor chosen by -i
   while its +0x538 is nonzero, else -1 (meaning unknown). */
s32 ScriptActor_GetField534FromScript(void)
{
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor && *(u16 *)(actor + 0x538))
        return *(u16 *)(actor + 0x534);
    return -1;
}
#else
INCLUDE_ASM("asm/nonmatching", ScriptActor_GetField534FromScript);
#endif
