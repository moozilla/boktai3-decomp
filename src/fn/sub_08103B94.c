#include "named_object.h"

#ifdef NONMATCHING
// fuzzy: 93.7%
struct NamedObject *NamedObject_FindByName(s32 name);
static inline u32 MovementFlag(s32 mode)
{
    return mode == 2 || mode == 4 ? mode : 0;
}
void NamedActor_SetMoveModeFromScript(void)
{
    s32 mode = Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    u32 highFlag = 4;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor && (actor->field_E0 & 2)) {
            s32 lowFlag = 2;
            actor->state = 2;
            if (mode == highFlag) {
                u16 bits = actor->flags & ~lowFlag;
                u32 flags = MovementFlag(mode) | bits;
                actor->flags = flags;
            } else {
                actor->flags &= ~highFlag;
                if (mode == 2)
                    actor->flags |= MovementFlag(mode);
                else
                    actor->flags &= ~lowFlag;
                NamedActor_PlayMovementSound(actor);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", NamedActor_SetMoveModeFromScript);
#endif
