#include "global.h"

extern void *gUnk_02000258;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081B8098(void *);
void Task_Kill(void *);
void sub_081B7C98(void);
void sub_081B7D44(void);

void *sub_081B80BC(void)
{
    void *r = Task_Create(8, 0x4c0);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081B7C98, sub_081B7D44);
        if (sub_081B8098(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    gUnk_02000258 = r;
    return r;
}
