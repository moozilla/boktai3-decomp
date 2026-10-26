#include "global.h"
#include "ram.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08122D58(u8 *);
void Task_Kill(u8 *);
void StateSpritePool_Tick(void);
void AnimatedBodyPool_Destroy(void);
void SceneScheduledEffect_EnsureSingleton(void)
{
    u8 *p;
    if (gAnimatedBodyPool == 0) {
        p = Task_Create(10, 0x550);
        if (p != 0) {
            Task_SetCallbacks(p, StateSpritePool_Tick, AnimatedBodyPool_Destroy);
            if (sub_08122D58(p) < 0)
                Task_Kill(p);
        }
    }
}
