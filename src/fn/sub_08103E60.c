#include "global.h"
#include "engine.h"
#include "ram.h"
u8 *NamedObject_FindByName(s32 name);
/* Script: for actor -n, a positive -h goes to +0xD8 and a positive -l to
   +0xD4 (tentative: high and low limits). */
void NamedActor_SetRangeFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor) {
            s32 none = -1;
            s32 h = Script_GetKeywordValue('h', none);
            s32 l = Script_GetKeywordValue('l', none);
            if (h > 0)
                *(s32 *)(actor + 0xD8) = h;
            if (l > 0)
                *(s32 *)(actor + 0xD4) = l;
        }
    }
}
