#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void ScreenMessage_Update(void);
void ScreenMessage_Destroy(void);
s32 ScreenMessage_Initialize(void *);

void *ScreenMessage_Create(void)
{
    void *r = Task_Create(0xb, 0x16c);
    if (r != 0) {
        Task_SetCallbacks(r, ScreenMessage_Update, ScreenMessage_Destroy);
        if (ScreenMessage_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
