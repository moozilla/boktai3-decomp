#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08046A10(void *, u32, u32);
void Task_Kill(void *);
void sub_0804694C(void);
void sub_08046A00(void);

void *sub_08046AD0(u32 a, u32 b)
{
    void *p = Task_Create(8, 0xC8);
    if (p) {
        Task_SetCallbacks(p, sub_0804694C, sub_08046A00);
        if (sub_08046A10(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
