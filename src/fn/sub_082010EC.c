#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0820107C(void *);
void Task_Kill(void *);
void sub_08200C94(void);
void sub_08201024(void);

void *sub_082010EC(void)
{
    void *p = Task_Create(8, 0xa2c);
    if (p) {
        Task_SetCallbacks(p, sub_08200C94, sub_08201024);
        if (sub_0820107C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
