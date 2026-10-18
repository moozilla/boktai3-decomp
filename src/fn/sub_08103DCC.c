#include "global.h"
#include "engine.h"
extern void *gUnk_020001AC;
u8 *NamedObject_FindByName(s32 name);
void NamedActor_SetSpeedFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gUnk_020001AC && name) {
        actor = NamedObject_FindByName(name);
        if (actor) {
            s32 speed = Script_GetKeywordValue('s', 6);
            *(u16 *)(actor + 0xBE) = speed;
        }
    }
}
