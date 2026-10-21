#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 Params3D0_InitDisplay(void *);
void Task_Kill(void *);
void Params3D0_UpdateDisplay(void);
void Params3D0_DestroyDisplay(void);

void *Params3D0_CreateDisplay(void)
{
    void *r = Task_Create(0xb, 1900);
    if (r != 0) {
        Task_SetCallbacks(r, Params3D0_UpdateDisplay, Params3D0_DestroyDisplay);
        if (Params3D0_InitDisplay(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
