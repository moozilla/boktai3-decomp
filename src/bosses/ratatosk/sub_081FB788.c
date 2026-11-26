#include "global.h"
void *SceneActors_GetBySlot(u32);
void *Task_Create(u32, u32);
void SceneDistance_UpdateActor(void);
void Ratatosk_Destroy(void);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081FAE0C(void *, void *);
void Task_Kill(void *);
void *Ratatosk_Create(void *arg)
{
    void *p = SceneActors_GetBySlot(4);
    if (p)
        return p;
    p = Task_Create(8, 0xf30);
    if (p) {
        Task_SetCallbacks(p, SceneDistance_UpdateActor, Ratatosk_Destroy);
        if (sub_081FAE0C(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
