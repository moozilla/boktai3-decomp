#include "global.h"
#include "ram.h"
u32 Script_GetKeywordValue(u32, u32);
void StateActor_CreateFromScriptEntry(void *, u32);
void StateActor_CreateFromScript(void)
{
    void *p = gSceneActorListManager;
    if (p != 0)
        StateActor_CreateFromScriptEntry(p, Script_GetKeywordValue(0x6e, 0));
}
