#include "global.h"
#include "engine.h"
#define NamedActor_SetSpeedFromScript sub_08103DCC
extern void *gUnk_020001AC;
u8 *sub_08101A3C(s32 name);
/* Script: actor -n gets -s (default 6) at +0xBE (tentative: route speed). */
void NamedActor_SetSpeedFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    if (gUnk_020001AC && name) {
        actor = sub_08101A3C(name);
        if (actor) {
            s32 speed = Script_GetKeywordValue('s', 6);
            *(u16 *)(actor + 0xBE) = speed;
        }
    }
}
