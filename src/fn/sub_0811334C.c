#include "global.h"

extern u8 *gUnk_020004C4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneDebrisPool_Initialize(u8 *);
void Task_Kill(u8 *);
void SceneDebrisPool_Update(void);
void SceneDebrisPool_Destroy(void);

u8 *SceneDebrisPool_EnsureSingleton(void)
{
    u8 *p;
    if (gUnk_020004C4 != 0)
        return gUnk_020004C4;
    p = Task_Create(0xa, 0x818);
    if (p != 0) {
        Task_SetCallbacks(p, SceneDebrisPool_Update, SceneDebrisPool_Destroy);
        if (SceneDebrisPool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
