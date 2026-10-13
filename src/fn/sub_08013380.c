#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 BreakableBarriers_Initialize(void *);
void Task_Kill(void *);
void sub_0801324C(void);
void sub_08013270(void);
void *sub_08013380(void)
{
    void *obj;
    obj = Task_Create(8, 0x2FC);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0801324C, sub_08013270);
        if (BreakableBarriers_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
