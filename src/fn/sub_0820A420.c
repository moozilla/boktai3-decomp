#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0820A3AC(void *);
void Task_Kill(void *);
void sub_0820A2C8(void);
void sub_0820A364(void);

void *sub_0820A420(void)
{
    void *p = Task_Create(8, 0xc1c);
    if (p) {
        Task_SetCallbacks(p, sub_0820A2C8, sub_0820A364);
        if (sub_0820A3AC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
