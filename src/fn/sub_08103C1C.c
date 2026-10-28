#include "global.h"
#include "ram.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *NamedObject_FindByName(s32);

void NamedActor_SetModeFromScript(void)
{
    s32 r = Script_GetKeywordValue(0x6e, 0);
    if (gNamedActorManager != 0 && r != 0) {
        u8 *p = NamedObject_FindByName(r);
        *(s32 *)(p + 0xe0) = Script_GetKeywordValue(0x6d, -1);
    }
}
