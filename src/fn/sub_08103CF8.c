#include "global.h"
#include "engine.h"
extern void *gUnk_020001AC;
u8 *sub_08101A3C(s32 name);
void Route_Start(void *route, u32 id, u32 point, u32 mode);
static inline u8 optionalByte(void)
{
    u8 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}
/* Script: actor -n (looked up by sub_08101A3C) starts following route -r id mode
   (Route_Start on its route block at +0xF4, which +0x128 then points to). */
void NamedActor_SetRouteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    u32 id, point, mode;
    if (gUnk_020001AC && name) {
        actor = sub_08101A3C(name);
        id = 0;
        point = 0;
        mode = 0;
        if (Script_SeekToKeyword('r')) {
            id = optionalByte();
            point = 0;
            mode = optionalByte();
        }
        Route_Start(actor + 0xF4, id, point, mode);
        *(void **)(actor + 0x128) = actor + 0xF4;
    }
}
