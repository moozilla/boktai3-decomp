#include "named_object.h"

struct NamedObject *NamedObject_FindByName(s32 name);

void NamedActor_SetMoveModeFromScript(void)
{
    s32 mode = Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;

    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor && (actor->field_E0 & 2)) {

            actor->state = 2;
            if (mode == 4) {
                NamedObject_DisableFlags(2, actor);
                NamedObject_EnableFlags(mode, actor);
            } else {
                NamedObject_DisableFlags(4, actor);
                if (mode == 2)
                    NamedObject_EnableFlags(mode, actor);
                else
                    NamedObject_DisableFlags(2, actor);
                NamedActor_PlayMovementSound(actor);
            }
        }
    }
}
