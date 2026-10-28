#include "global.h"
#include "engine.h"
#include "ram.h"
u8 *NamedObject_FindByName(s32 name);
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
/* Script: actor -n (looked up by NamedObject_FindByName) starts following route -r id mode
   (Route_Start on its route block at +0xF4, which +0x128 then points to). */
void NamedActor_SetRouteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    u8 *actor;
    u32 id, point, mode;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
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
