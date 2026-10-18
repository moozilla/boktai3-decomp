#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ShadowTask_Init(void *, void *);
void Task_Kill(void *);
void ShadowTask_Update(void);
void ShadowTask_Destroy(void);

void *ShadowTask_Create(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, ShadowTask_Update, ShadowTask_Destroy);
        if (ShadowTask_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
