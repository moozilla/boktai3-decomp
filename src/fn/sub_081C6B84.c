#include "global.h"
extern void *gUnk_02000480;
u32 Script_GetKeywordValue(u32, u32);
void StateActor_CreateFromScriptEntry(void *, u32);
void StateActor_CreateFromScript(void)
{
    void *p = gUnk_02000480;
    if (p != 0)
        StateActor_CreateFromScriptEntry(p, Script_GetKeywordValue(0x6e, 0));
}
