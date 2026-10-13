#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08018E00(void *);
void Task_Kill(void *);
void BlendTransition_Update(void);
void sub_08018DFC(void);
void *sub_08018EF0(void)
{
    void *obj;
    obj = Task_Create(11, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, BlendTransition_Update, sub_08018DFC);
        if (sub_08018E00(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
