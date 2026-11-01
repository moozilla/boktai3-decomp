#include "global.h"
#include "engine.h"
#include "ram.h"
#include "named_object_flags.h"
struct NamedActor {
    u8 pad0[0xB6];
    u16 state;
    struct NamedObjectFlags flags;
    u8 padBA[0x26];
    u32 flagsE0;
};
struct NamedActor *NamedObject_FindByName(s32 name);
void NamedActor_PlayMovementSound(struct NamedActor *);
/* Script: for actor -n with flag 2 at +0xE0: state = 2, then by -m (default
   1): 4 sets flag 4 and clears 2; otherwise flag 4 is cleared, flag 2 set for
   2 or cleared, and NamedActor_PlayMovementSound runs (tentative). */
void NamedActor_SetMoveModeFromScript(void)
{
    u32 mode = Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedActor *actor;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor && (actor->flagsE0 & 2)) {
            s32 two;
            actor->state = 2;
            two = 2;
            if (mode == 4) {
                NamedObjectFlags_Clear(&actor->flags, two);
                NamedObjectFlags_Set(&actor->flags, mode);
            } else {
                NamedObjectFlags_Clear(&actor->flags, 4);
                if (mode == 2)
                    NamedObjectFlags_Set(&actor->flags, mode);
                else
                    NamedObjectFlags_Clear(&actor->flags, two);
                NamedActor_PlayMovementSound(actor);
            }
        }
    }
}
