#include "global.h"
#include "ram.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *NamedObject_FindByName(s32);
void NamedActor_ClearFlag1000FromScript(void)
{
    s32 r = Script_GetKeywordValue(0x6e, 0);
    if (gNamedActorManager != 0 && r != 0) {
        u8 *p = NamedObject_FindByName(r);
        if (p != 0) {
            u16 *q = (u16 *)(p + 0xb8);
            u32 m = 0xFFFFEFFF;
            *q = m & *q;
        }
    }
}