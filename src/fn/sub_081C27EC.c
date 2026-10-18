#include "global.h"
extern u32 gUnk_02000260;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C2570(void *);
void Task_Kill(void *);
u32 ControlPanel_OnTaskUpdate(void *);
void sub_081C22BC(void *);
void *ControlPanel_CreateFromScript(void)
{
    void *p;
    if (gUnk_02000260 != 0)
        return (void *)gUnk_02000260;
    p = Task_Create(8, 0x5bc);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))ControlPanel_OnTaskUpdate, sub_081C22BC);
        if (sub_081C2570(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
