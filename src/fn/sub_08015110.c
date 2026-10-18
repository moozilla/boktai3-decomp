#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 FragmentManager_Initialize(void *, void *);
void Task_Kill(void *);
void FragmentManager_Update(void);
void FragmentManager_Destroy(void);

void *FragmentManager_Create(void *a)
{
    void *p = Task_Create(10, 0x600);
    if (p != 0) {
        Task_SetCallbacks(p, FragmentManager_Update, FragmentManager_Destroy);
        if (FragmentManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
