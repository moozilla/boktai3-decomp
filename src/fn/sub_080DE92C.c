#include "global.h"
#include "scene_actor_profile.h"

void SceneActor_ConfigureSecondaryProfileEntry(struct SceneActorRegistryActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->profileIndex, actor->livingCountByCategory, 0);

    {
        u16 mask = 0x280;

        u16 v = actor->flags2D4 & mask;

        if (v) {
            u8 *q;
            u32 z;
            u32 four = 4;
            z = 0;
            q = &entry[1].value;
            entry[1].value = four;
            q[1] = z;
        } else {
            entry[1].value = 3;
            entry[1].flags = v;
        }
    }

    {
        u16 mask = 0x200;

        if (actor->flags2D4 & mask)
            actor->field2C2 = 0x20;
    }
}
