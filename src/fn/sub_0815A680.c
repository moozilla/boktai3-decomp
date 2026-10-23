#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 65.4%
// fix.py best version (reprinted C)
// Open: the original keeps two copies of the actor pointer (r4 and r5).
extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
/* Script: tests flag -f (0-95) in the three flag words at +0x444 of the
   scripted actor chosen by -i; returns the masked word (nonzero if set). */
u32 ScriptActor_TestFlagFromScript(void)
{
    u8 *actor;
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = (actor = gUnk_02000580[i]);
    if (actor && Script_SeekToKeyword('f')) {
        u32 flag = Script_GetValue();
        u32 *word;
        u8 mask;
        if (flag >= 64) {
            word = (u32 *)(actor + 0x44C);
            mask = 1 << (flag - 64);
        } else if (flag >= 32) {
            word = (u32 *)(actor + 0x448);
            mask = 1 << (flag - 32);
        } else {
            word = (u32 *)(p + 0x444);
            mask = 1 << flag;
        }
        return mask & (*word);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", ScriptActor_TestFlagFromScript);
#endif
