#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081DC244(void);
void sub_081DC448(void);
s32 sub_081DC470(void *);

void *sub_081DC690(void)
{
    void *r = Task_Create(0xb, 0x16c);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DC244, sub_081DC448);
        if (sub_081DC470(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
