#include "scene/collision/contact_swarm.h"
s32 CountBats_Initialize(struct ContactSwarm *);
u32 CountBats_Update(struct ContactSwarm *);
u32 CountBats_Destroy(struct ContactSwarm *);
struct ContactSwarm *CountBats_Create(void)
{
    struct ContactSwarm *pool = Task_Create(8, 0x1590);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))CountBats_Update,
                          (void (*)(void *))CountBats_Destroy);
        if (CountBats_Initialize(pool) >= 0)
            goto done;
        Task_Kill(pool);
        return 0;
    }
done:
    return pool;
}
