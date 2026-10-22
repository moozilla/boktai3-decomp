#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void NameEntry_Update(void);
void NameEntry_Destroy(void);
s32 NameEntry_Initialize(void *);

void *NameEntry_Create(void)
{
    void *r = Task_Create(0xb, 0x3e4);
    if (r != 0) {
        Task_SetCallbacks(r, NameEntry_Update, NameEntry_Destroy);
        if (NameEntry_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
