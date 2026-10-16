#include "global.h"
#include "engine.h"
extern void *gUnk_020001AC;
u8 *sub_08101A3C(s32 name);
static inline void orFlags(u16 mask, u16 *flags)
{
    *flags |= mask;
}
/* Script: sets flag 0x20 at +0xB8 of actor -n. */
void NamedActor_SetFlag20FromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gUnk_020001AC && name) {
        actor = sub_08101A3C(name);
        if (actor)
            orFlags(0x20, (u16 *)(actor + 0xB8));
    }
}
