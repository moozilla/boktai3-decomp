#include "global.h"

extern u8 *gUnk_0200012C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneColliderEffect_InitPool(u8 *);
void Task_Kill(u8 *);
void sub_08121A78(void);
void SceneColliderEffect_DestroyPool(void);

u8 *SceneColliderEffect_EnsurePool(void)
{
    u8 *p;
    if (gUnk_0200012C != 0)
        return gUnk_0200012C;
    p = Task_Create(0xa, 0x2cc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08121A78, SceneColliderEffect_DestroyPool);
        if (SceneColliderEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
