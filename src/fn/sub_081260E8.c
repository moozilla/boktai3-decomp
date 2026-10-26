#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneMovingParticle_InitPool(u8 *);
void Task_Kill(u8 *);
void sub_08126000(void);
void SceneMovingParticle_OnPoolDestroy(void);

u8 *SceneMovingParticle_EnsurePool(void)
{
    u8 *p;
    if (gSpriteBurstManager != 0)
        return gSpriteBurstManager;
    p = Task_Create(0xa, 0x6a0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08126000, SceneMovingParticle_OnPoolDestroy);
        if (SceneMovingParticle_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
