#include "global.h"
extern u32 gUnk_03002604;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Solar_InitController(void *);
void Task_Kill(void *);
void Solar_UpdateTask(void);
void Solar_DestroyController(void);

void *Solar_CreateController(void)
{
    void *p;
    if (gUnk_03002604 != 0)
        return (void *)gUnk_03002604;
    p = Task_Create(5, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, Solar_UpdateTask, Solar_DestroyController);
        if (Solar_InitController(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
