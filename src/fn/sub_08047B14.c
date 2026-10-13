#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08047A30(void *, u32, u32);
void Task_Kill(void *);
void sub_080479F4(void);
void sub_08047A2C(void);

void *sub_08047B14(u32 a, u32 b)
{
    void *p = Task_Create(9, 0x48);
    if (p) {
        Task_SetCallbacks(p, sub_080479F4, sub_08047A2C);
        if (sub_08047A30(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
