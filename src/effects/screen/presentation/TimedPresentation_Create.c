#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 TimedPresentation_Initialize(void *);
void Task_Kill(void *);
void TimedPresentation_Update(void);
void TimedPresentation_Destroy(void);

void *TimedPresentation_Create(void)
{
    void *r = Task_Create(0xb, 0x38);
    if (r != 0) {
        Task_SetCallbacks(r, TimedPresentation_Update, TimedPresentation_Destroy);
        if (TimedPresentation_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
