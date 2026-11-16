#include "global.h"
#include "scene_actor_profile.h"

void SceneActor_ConfigureSecondaryProfileEntry(struct ChaseActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);

    {
        u16 mask = 0x280;

        u16 v = actor->base.flags2D4 & mask;

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

        if (actor->base.flags2D4 & mask)
            actor->base.field2C2 = 0x20;
    }
}
