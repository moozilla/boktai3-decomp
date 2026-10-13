#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 FragmentManager_Initialize(void *, void *);
void Task_Kill(void *);
void sub_08015040(void);
void sub_08015074(void);

void *sub_08015110(void *a)
{
    void *p = Task_Create(10, 0x600);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08015040, sub_08015074);
        if (FragmentManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
