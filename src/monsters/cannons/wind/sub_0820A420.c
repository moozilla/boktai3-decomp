#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 WindCannonManager_Init(void *);
void Task_Kill(void *);
void WindCannonManager_Update(void);
void WindCannonManager_Destroy(void);

void *WindCannonManager_Create(void)
{
    void *p = Task_Create(8, 0xc1c);
    if (p) {
        Task_SetCallbacks(p, WindCannonManager_Update, WindCannonManager_Destroy);
        if (WindCannonManager_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
