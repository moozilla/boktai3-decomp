#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FB8A0(void);
void sub_081FBAA8(void);
s32 sub_081FBAE0(void *);
void *sub_081FBB38(void)
{
    void *r = Task_Create(8, 0xb20);
    if (r) {
        Task_SetCallbacks(r, sub_081FB8A0, sub_081FBAA8);
        if (sub_081FBAE0(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
