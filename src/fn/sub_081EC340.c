#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081EC1A8(void);
void sub_081EC1F8(void);
s32 sub_081EC204(void *);

void *Results_Create(void)
{
    void *r = Task_Create(0xb, 0x138);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081EC1A8, sub_081EC1F8);
        if (sub_081EC204(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
