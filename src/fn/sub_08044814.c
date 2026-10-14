#include "contact_swarm.h"
s32 ContactSwarm_Initialize(struct ContactSwarm *);
u32 ContactSwarm_Update(struct ContactSwarm *);
u32 ContactSwarm_Destroy(struct ContactSwarm *);
struct ContactSwarm *ContactSwarm_Create(void)
{
    struct ContactSwarm *pool = Task_Create(8, 0x1590);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))ContactSwarm_Update,
                          (void (*)(void *))ContactSwarm_Destroy);
        if (ContactSwarm_Initialize(pool) >= 0)
            goto done;
        Task_Kill(pool);
        return 0;
    }
done:
    return pool;
}
