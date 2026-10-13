#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Time_LoadRegionList(void *);
void Task_Kill(void *);
void Time_HandleRegionKeypad(void);
void Time_DestroyRegionMenu(void);
void *sub_081DCDF4(void)
{
    void *p = Task_Create(0xb, 0x5b8);
    if (p) {
        Task_SetCallbacks(p, Time_HandleRegionKeypad, Time_DestroyRegionMenu);
        if (Time_LoadRegionList(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
