#include "global.h"
extern u32 gUnk_02000260;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C2570(void *);
void Task_Kill(void *);
u32 sub_081C2294(void *);
void sub_081C22BC(void *);
void *sub_081C27EC(void)
{
    void *p;
    if (gUnk_02000260 != 0)
        return (void *)gUnk_02000260;
    p = Task_Create(8, 0x5bc);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))sub_081C2294, sub_081C22BC);
        if (sub_081C2570(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
