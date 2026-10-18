#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081EB8E0(void);
void ScreenParams3F8_DestroyDisplay(void);
s32 sub_081EBD0C(void *);

void *ScreenParams3F8_CreateDisplay(void)
{
    void *r = Task_Create(0xb, 0x214);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081EB8E0, ScreenParams3F8_DestroyDisplay);
        if (sub_081EBD0C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
