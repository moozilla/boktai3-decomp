#include "temporary_animation_pool.h"
#define TemporaryAnimationPool_Create sub_08047CB8
#define TemporaryAnimationPool_Update sub_08047BB4
#define TemporaryAnimationPool_Destroy sub_08047BE8
#define TemporaryAnimationPool_Initialize sub_08047C14
s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *);
struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void)
{
    struct TemporaryAnimationPool *pool = gUnk_020000A8;
    if (!pool) {
        pool = Task_Create(10, sizeof(*pool));
        if (pool) {
            Task_SetCallbacks(pool, (void (*)(void *))TemporaryAnimationPool_Update, (void (*)(void *))TemporaryAnimationPool_Destroy);
            if (TemporaryAnimationPool_Initialize(pool) < 0) {
                Task_Kill(pool);
                return NULL;
            }
        }
    }
    return pool;
}
