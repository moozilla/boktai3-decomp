#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 ResultTutorial_Initialize(void *, void *, void *);
void Task_Kill(void *);
void sub_0801DE98(void);
void sub_0801CF60(void);
void *sub_0801DFB8(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0x7F4);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0801DE98, sub_0801CF60);
        if (ResultTutorial_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
