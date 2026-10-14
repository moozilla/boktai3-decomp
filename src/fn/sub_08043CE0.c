#include "contact_actor.h"
s32 ContactActorPool_Initialize(struct ContactActorPool *, u32, u32, u32);
u32 ContactActorPool_Update(struct ContactActorPool *);
u32 ContactActorPool_Destroy(u8 *);
struct ContactActorPool *ContactActorPool_Create(u32 mode, u32 resourceId, u32 healthId)
{
    struct ContactActorPool *pool = Task_Create(8, 0x1720);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))ContactActorPool_Update,
                          (void (*)(void *))ContactActorPool_Destroy);
        if (ContactActorPool_Initialize(pool, mode, resourceId, healthId) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
