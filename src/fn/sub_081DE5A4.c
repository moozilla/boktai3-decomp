#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081DDBC0(void);
void sub_081DE078(void);
s32 sub_081DE0CC(void *);

void *NameEntry_Create(void)
{
    void *r = Task_Create(0xb, 0x3e4);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DDBC0, sub_081DE078);
        if (sub_081DE0CC(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
