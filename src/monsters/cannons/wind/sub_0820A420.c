#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 FanCannonManager_Init(void *);
void Task_Kill(void *);
void FanCannonManager_Update(void);
void FanCannonManager_Destroy(void);

void *FanCannonManager_Create(void)
{
    void *p = Task_Create(8, 0xc1c);
    if (p) {
        Task_SetCallbacks(p, FanCannonManager_Update, FanCannonManager_Destroy);
        if (FanCannonManager_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
