#include "contact_actor.h"
#define ContactActorPool_Create sub_08043CE0
s32 sub_08043C4C(struct ContactActorPool *, u32, u32, u32);
void sub_08043ACC(void *);
void sub_08043C24(void *);
struct ContactActorPool *ContactActorPool_Create(u32 mode, u32 resourceId, u32 healthId)
{
    struct ContactActorPool *pool = Task_Create(8, 0x1720);
    if (pool) {
        Task_SetCallbacks(pool, sub_08043ACC, sub_08043C24);
        if (sub_08043C4C(pool, mode, resourceId, healthId) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
