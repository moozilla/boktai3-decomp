#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081602D0(void *, void *);
void Task_Kill(void *);
void PlayerBurst_Update(void);
void PlayerBurst_Destroy(void);

void *PlayerBurst_Create(void *p)
{
    void *r = Task_Create(0xb, 0x4C8);
    if (r != 0) {
        Task_SetCallbacks(r, PlayerBurst_Update, PlayerBurst_Destroy);
        if (sub_081602D0(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
