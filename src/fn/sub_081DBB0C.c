#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Options_UpdateScreen(void);
void Options_Destroy(void);
s32 Options_InitScreen(void *);

void *Options_Create(void)
{
    void *r = Task_Create(0xb, 0x304);
    if (r != 0) {
        Task_SetCallbacks(r, Options_UpdateScreen, Options_Destroy);
        if (Options_InitScreen(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
