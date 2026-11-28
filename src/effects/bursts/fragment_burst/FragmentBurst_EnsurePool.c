#include "effects/bursts/fragment_burst.h"

s32 FragmentBurst_InitPool(struct FragmentBurstPool *);
s32 SpritePool4_UpdateActive(struct FragmentBurstPool *);
struct FragmentBurstPool *SpriteGroupPool_Destroy(struct FragmentBurstPool *);

struct FragmentBurstPool *FragmentBurst_EnsurePool(void)
{
    struct FragmentBurstPool *pool;

    if (gSpriteGroupPool)
        return gSpriteGroupPool;
    pool = Task_Create(0xa, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, SpritePool4_UpdateActive, SpriteGroupPool_Destroy);
        if (FragmentBurst_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
