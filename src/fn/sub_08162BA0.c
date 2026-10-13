#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08162A50(void *, void *);
void Task_Kill(void *);
void sub_081627A4(void);
void sub_08162A30(void);

void *sub_08162BA0(void *a)
{
    void *r = Task_Create(8, 0x150);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081627A4, sub_08162A30);
        if (sub_08162A50(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
