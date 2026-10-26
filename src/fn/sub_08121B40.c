#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneColliderEffect_InitPool(u8 *);
void Task_Kill(u8 *);
void ActionGroup_RunSelectedCallbacks(void);
void SceneColliderEffect_DestroyPool(void);

u8 *SceneColliderEffect_EnsurePool(void)
{
    u8 *p;
    if (gColliderEffectPool != 0)
        return gColliderEffectPool;
    p = Task_Create(0xa, 0x2cc);
    if (p != 0) {
        Task_SetCallbacks(p, ActionGroup_RunSelectedCallbacks, SceneColliderEffect_DestroyPool);
        if (SceneColliderEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
