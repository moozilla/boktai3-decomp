#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08204F40(void);
void sub_08205390(void);
s32 sub_0820539C(void *, void *);
void *sub_08205900(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, sub_08204F40, sub_08205390);
        if (sub_0820539C(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
