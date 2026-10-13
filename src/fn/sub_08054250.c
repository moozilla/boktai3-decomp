#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08054028(void *, u32, u32);
void Task_Kill(void *);
void sub_08053FF8(void);
void sub_0805400C(void);

void *sub_08054250(u32 a, u32 b)
{
    void *p = Task_Create(9, 0xCC);
    if (p) {
        Task_SetCallbacks(p, sub_08053FF8, sub_0805400C);
        if (sub_08054028(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
