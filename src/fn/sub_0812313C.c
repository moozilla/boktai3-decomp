#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneTrailEffect_InitPool(u8 *);
void Task_Kill(u8 *);
void SpriteNotice_TickManager(void);
void SceneAnimatedEffect_OnTaskDestroy(void);

u8 *SceneTrailEffect_EnsurePool(void)
{
    u8 *p;
    if (gSpriteNoticeManager != 0)
        return gSpriteNoticeManager;
    p = Task_Create(0xa, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, SpriteNotice_TickManager, SceneAnimatedEffect_OnTaskDestroy);
        if (SceneTrailEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
