#include "burst_emitter.h"
#define BurstEmitter_CreatePool sub_08069F4C
s32 sub_08069ED0(struct BurstEmitterPool *);
void sub_08069F24(void *);
s32 sub_08069F34(struct BurstEmitterPool *, u32);
struct BurstEmitterPool *BurstEmitter_CreatePool(void)
{
    struct BurstEmitterPool *pool;
    if (gUnk_02000160)
        return gUnk_02000160;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void *)sub_08069ED0, (void *)sub_08069F24);
        if (sub_08069F34(pool, 0) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
