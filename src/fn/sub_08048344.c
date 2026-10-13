#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08048248(void *, void *);
void Task_Kill(void *);
void sub_080480A4(void);
void sub_0804822C(void);

void *sub_08048344(void *a)
{
    void *p = Task_Create(8, 0xC4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080480A4, sub_0804822C);
        if (sub_08048248(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
