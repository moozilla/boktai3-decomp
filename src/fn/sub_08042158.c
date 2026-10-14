#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ModifierMenu_Initialize(void *);
void Task_Kill(void *);
void ModifierMenu_UpdateTask(void);
void sub_08042044(void);
void *sub_08042158(void)
{
    void *p = Task_Create(4, 0x1088);
    if (p) {
        Task_SetCallbacks(p, ModifierMenu_UpdateTask, sub_08042044);
        if (ModifierMenu_Initialize(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
