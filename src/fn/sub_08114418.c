#include "global.h"

extern u8 *gUnk_020004C8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneCallbackPool_Initialize(u8 *);
void Task_Kill(u8 *);
void SceneCallbackPool_Update(void);
void NativeEffect_DestroyManager(void);

u8 *NativeEffect_EnsureManager(void)
{
    u8 *p;
    if (gUnk_020004C8 != 0)
        return gUnk_020004C8;
    p = Task_Create(0xa, 0xd9c);
    if (p != 0) {
        Task_SetCallbacks(p, SceneCallbackPool_Update, NativeEffect_DestroyManager);
        if (SceneCallbackPool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
