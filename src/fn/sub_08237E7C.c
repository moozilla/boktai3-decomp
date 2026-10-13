#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08237E40(void *, s32, s32, s32);
void Task_Kill(void *);
void sub_08237D68(void);
void sub_08237D7C(void);

void *sub_08237E7C(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x140);
    if (r != 0) {
        Task_SetCallbacks(r, sub_08237D68, sub_08237D7C);
        if (sub_08237E40(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
