#include "objects/bouncing_bombs/bouncing_object.h"
s32 BouncingBomb_InitializePool(struct BouncingPool *);
s32 BouncingBomb_UpdatePool(struct BouncingPool *);
s32 BouncingBomb_DestroyPool(struct BouncingPool *);
struct BouncingPool *BouncingBomb_EnsurePool(void)
{
    struct BouncingPool *pool;
    if (gBouncingPool)
        return gBouncingPool;
    pool = Task_Create(8, 0xFD0);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))BouncingBomb_UpdatePool,
                          (void (*)(void *))BouncingBomb_DestroyPool);
        if (BouncingBomb_InitializePool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
