#include "global.h"
extern u8 *gUnk_02000580[];
extern u8 *gUnk_02000710;
s32 Script_GetActorIndex(void);
void ScriptActor_ClearCounter628FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p) {
        u32 k = p[0x472];
        u8 *b = gUnk_02000710;
        *(u32 *)(b + k * 4 + 0x628) = 0;
    }
}
