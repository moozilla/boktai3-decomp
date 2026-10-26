#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 BlinkingEffect_InitPool(u8 *);
void Task_Kill(u8 *);
void SceneStatusIndicator_UpdateManager(void);
void BlinkingEffect_DestroyPool(void);

u8 *BlinkingEffect_EnsurePool(void)
{
    u8 *p;
    if (gSceneStatusIndicatorManager != 0)
        return gSceneStatusIndicatorManager;
    p = Task_Create(0xa, 0x260);
    if (p != 0) {
        Task_SetCallbacks(p, SceneStatusIndicator_UpdateManager, BlinkingEffect_DestroyPool);
        if (BlinkingEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
