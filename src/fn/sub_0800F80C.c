#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0800F6F8(void *, u16);
void Task_Kill(void *);
void sub_0800F6E0(void);
void sub_0800F6EC(void);
void *sub_0800F80C(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, sub_0800F6E0, sub_0800F6EC);
        if (sub_0800F6F8(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
