#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 TitleDisplay_Initialize(void *);
void Task_Kill(void *);
void TitleDisplay_Update(void);
void TitleDisplay_Destroy(void *);

void *TitleDisplay_Create(void)
{
    void *r = Task_Create(0xb, 1900);
    if (r != 0) {
        Task_SetCallbacks(r, TitleDisplay_Update, TitleDisplay_Destroy);
        if (TitleDisplay_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
