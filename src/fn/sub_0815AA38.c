#include "global.h"
extern u8 *gUnk_02000580[];
extern u8 *gUnk_02000710;
s32 Script_GetActorIndex(void);
u32 ScriptActor_TestCounter628FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p != 0) {
        u32 k = p[0x472];
        u8 *b = gUnk_02000710;
        if (*(s32 *)(b + k * 4 + 0x628) > 0)
            goto one;
    }
    return 0;
one:
    return 1;
}
