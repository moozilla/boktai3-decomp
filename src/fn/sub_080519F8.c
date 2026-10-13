#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08051858(void *, u32, u32);
void Task_Kill(void *);
void sub_080516FC(void);
void sub_0805173C(void);

void *sub_080519F8(u32 a, u32 b)
{
    void *p = Task_Create(8, 0xD0);
    if (p) {
        Task_SetCallbacks(p, sub_080516FC, sub_0805173C);
        if (sub_08051858(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
