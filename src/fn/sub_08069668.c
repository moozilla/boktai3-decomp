#include "bouncing_object.h"
#define BouncingObject_EnsurePool sub_08069668
s32 sub_08069620(struct BouncingPool *);
s32 sub_08069510(struct BouncingPool *);
s32 sub_080695C8(struct BouncingPool *);
struct BouncingPool *BouncingObject_EnsurePool(void)
{
    struct BouncingPool *pool;
    if (gUnk_0200014C) return gUnk_0200014C;
    pool = Task_Create(8, 0xFD0);
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))sub_08069510,
                          (void (*)(void *))sub_080695C8);
        if (sub_08069620(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
