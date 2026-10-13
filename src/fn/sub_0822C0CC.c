#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Task_Kill(void *);
void sub_0822C07C(void *), sub_0822C080(void *);
s32 sub_0822C098(void *, u32, u32);
void *sub_0822C0CC(u32 a, u32 b)
{
    void *obj = Task_Create(4, 88);
    if (obj) {
        Task_SetCallbacks(obj, sub_0822C07C, sub_0822C080);
        if (sub_0822C098(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
