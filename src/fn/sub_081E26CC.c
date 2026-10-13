#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 SoundTest_Initialize(void *);
void Task_Kill(void *);
void SoundTest_Update(void);
void SoundTest_Destroy(void);

void *sub_081E26CC(void)
{
    void *r = Task_Create(0xb, 1572);
    if (r != 0) {
        Task_SetCallbacks(r, SoundTest_Update, SoundTest_Destroy);
        if (SoundTest_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
