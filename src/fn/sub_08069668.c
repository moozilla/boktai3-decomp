#include "bouncing_object.h"
s32 BouncingObject_InitializePool(struct BouncingPool *);
s32 BouncingObject_UpdatePool(struct BouncingPool *);
s32 BouncingObject_DestroyPool(struct BouncingPool *);
struct BouncingPool *BouncingObject_EnsurePool(void)
{
    struct BouncingPool *pool;
    if (gUnk_0200014C)
        return gUnk_0200014C;
    pool = Task_Create(8, 0xFD0);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))BouncingObject_UpdatePool,
                          (void (*)(void *))BouncingObject_DestroyPool);
        if (BouncingObject_InitializePool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
