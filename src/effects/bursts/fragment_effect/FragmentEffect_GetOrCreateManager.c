#include "effects/bursts/fragment_effect.h"

s32 FragmentEffect_InitManager(struct FragmentEffectPool *);
u32 FragmentEffect_UpdateManager(struct FragmentEffectPool *);
u32 FragmentEffect_DestroyManager(struct FragmentEffectPool *);

struct FragmentEffectPool *FragmentEffect_GetOrCreateManager(void)
{
    struct FragmentEffectPool *pool;

    if (gFragmentEffectManager)
        return (struct FragmentEffectPool *)gFragmentEffectManager;
    pool = Task_Create(0xa, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, FragmentEffect_UpdateManager, FragmentEffect_DestroyManager);
        if (FragmentEffect_InitManager(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
