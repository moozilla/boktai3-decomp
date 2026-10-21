// MATCHING_CONCESSION: register-pinning
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
            register u8 *q asm("r1");
            register u32 z asm("r2");
            register u32 four asm("r0") = 4;
            asm("" : "+r"(four));
            z = 0;
            q = &entry[1].value;
            asm("" : "+r"(z), "+r"(q));
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
