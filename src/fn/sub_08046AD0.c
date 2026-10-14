#include "orbit_contact_actor.h"
s32 OrbitContactActor_Initialize(struct OrbitContactActor *, const struct ContactPosition *, u32);
s32 OrbitContactActor_Update(struct OrbitContactActor *);
s32 OrbitContactActor_Destroy(struct OrbitContactActor *);
struct OrbitContactActor *OrbitContactActor_Create(const struct ContactPosition *center,
                                                   u32 resourceValue)
{
    struct OrbitContactActor *actor = Task_Create(8, sizeof(*actor));
    if (actor) {
        Task_SetCallbacks(actor, (void (*)(void *))OrbitContactActor_Update,
                          (void (*)(void *))OrbitContactActor_Destroy);
        if (OrbitContactActor_Initialize(actor, center, resourceValue) < 0) {
            Task_Kill(actor);
            return NULL;
        }
    }
    return actor;
}
