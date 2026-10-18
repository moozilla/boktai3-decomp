#include "global.h"

extern u8 *gUnk_02000144;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneTrailEffect_InitPool(u8 *);
void Task_Kill(u8 *);
void sub_08123044(void);
void SceneAnimatedEffect_OnTaskDestroy(void);

u8 *SceneTrailEffect_EnsurePool(void)
{
    u8 *p;
    if (gUnk_02000144 != 0)
        return gUnk_02000144;
    p = Task_Create(0xa, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08123044, SceneAnimatedEffect_OnTaskDestroy);
        if (SceneTrailEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
