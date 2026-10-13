#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08053AEC(void *, u32, u32);
void Task_Kill(void *);
void sub_08053AB0(void);
void sub_08053AC4(void);

void *sub_08053D44(u32 a, u32 b)
{
    void *p = Task_Create(9, 0x118);
    if (p) {
        Task_SetCallbacks(p, sub_08053AB0, sub_08053AC4);
        if (sub_08053AEC(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
