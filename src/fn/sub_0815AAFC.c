#include "global.h"
extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
void ScriptActor_SetField4A2FromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p)
        *(u16 *)(p + 0x4a2) = 1;
}
