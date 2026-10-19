#include "global.h"
#include "scene_actor_profile.h"

#ifdef NONMATCHING
// fuzzy: 93.9%
// foundry draft (worker best 93.9%); what differs is mostly register allocation and instruction order
#define SceneActor_ConfigureSecondaryProfileEntry sub_080DE92C

void SceneActor_ConfigureSecondaryProfileEntry(struct SceneActorRegistryActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->profileIndex, actor->livingCountByCategory, 0);

    {
        u16 mask = 0x280;

        if (actor->flags2D4 & mask) {
            entry[1].value = 4;
            entry[1].flags = 0;
        } else {
            entry[1].value = 3;
            entry[1].flags = 0;
        }
    }

    {
        u16 mask = 0x200;

        if (actor->flags2D4 & mask)
            actor->field2C2 = 0x20;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DE92C);
#endif
