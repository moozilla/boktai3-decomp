#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 PaletteFade_InitFromScript(u8 *);
void Task_Kill(u8 *);
void PaletteFade_OnTaskUpdate(void);
void PaletteFade_OnTaskDestroy(void);

u8 *PaletteFade_CreateFromScript(void)
{
    u8 *p;
    p = Task_Create(9, 0x1ae4);
    if (p != 0) {
        Task_SetCallbacks(p, PaletteFade_OnTaskUpdate, PaletteFade_OnTaskDestroy);
        if (PaletteFade_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
