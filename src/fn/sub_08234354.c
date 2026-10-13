#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_082342AC(void *, s32, s32, s32);
void Task_Kill(void *);
void sub_082340A0(void);
void sub_082340B4(void);

void *sub_08234354(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x1e0);
    if (r != 0) {
        Task_SetCallbacks(r, sub_082340A0, sub_082340B4);
        if (sub_082342AC(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
