#include "global.h"
#include "engine.h"
extern void *gUnk_020001AC;
u8 *sub_08101A3C(s32 name);
/* Script: for actor -n, a positive -h goes to +0xD8 and a positive -l to
   +0xD4 (tentative: high and low limits). */
void NamedActor_SetRangeFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gUnk_020001AC && name) {
        actor = sub_08101A3C(name);
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
