#include "global.h"
#include "engine/ram.h"
u32 Script_GetKeywordValue(u32, u32);
void StoryCharacter_CreateFromScriptEntry(void *, u32);
void StoryCharacter_CreateFromScript(void)
{
    void *p = gSceneActorListManager;
    if (p != 0)
        StoryCharacter_CreateFromScriptEntry(p, Script_GetKeywordValue(0x6e, 0));
}
