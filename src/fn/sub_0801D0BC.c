#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 ResultMenu_Initialize(void *, void *, void *);
void Task_Kill(void *);
void ResultMenu_Update(void);
void sub_0801CF60(void);
void *sub_0801D0BC(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0x7F4);
    if (obj != 0) {
        Task_SetCallbacks(obj, ResultMenu_Update, sub_0801CF60);
        if (ResultMenu_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
