#include "global.h"
#include "ram.h"
struct P {
    u8 f[0x18];
    u32 w;
};
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void SceneEffectQueue_UpdateEffects(void);
void CallbackPool_Destroy(void);
s32 SceneEffect_InitializeEmitterOptions(void *);
struct P *SceneEffectQueue_Create(u16 a)
{
    struct P *p;
    if (gSceneEffectQueue != 0)
        return 0;
    p = Task_Create(8, 0x46C);
    if (p != 0) {
        p->w = a;
        Task_SetCallbacks(p, SceneEffectQueue_UpdateEffects, CallbackPool_Destroy);
        if (SceneEffect_InitializeEmitterOptions(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        gSceneEffectQueue = p;
    }
    return p;
}
