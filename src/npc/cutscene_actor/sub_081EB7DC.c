#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 CutsceneDisplay_InitDisplay(void *);
void Task_Kill(void *);
void CutsceneDisplay_UpdateDisplay(void);
void CutsceneDisplay_DestroyDisplay(void *);

void *CutsceneDisplay_CreateDisplay(void)
{
    void *r = Task_Create(0xb, 1900);
    if (r != 0) {
        Task_SetCallbacks(r, CutsceneDisplay_UpdateDisplay, CutsceneDisplay_DestroyDisplay);
        if (CutsceneDisplay_InitDisplay(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
