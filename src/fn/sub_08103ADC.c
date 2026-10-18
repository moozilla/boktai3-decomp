#include "global.h"
#include "engine.h"
extern void *gUnk_020001AC;
u8 *NamedObject_FindByName(s32 name);
static inline void orFlags(u16 mask, u16 *flags)
{
    *flags |= mask;
}
void NamedActor_SetFlag20FromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gUnk_020001AC && name) {
        actor = NamedObject_FindByName(name);
        if (actor)
            orFlags(0x20, (u16 *)(actor + 0xB8));
    }
}
