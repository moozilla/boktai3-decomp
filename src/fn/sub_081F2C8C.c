#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 TimedSprite_Init(void *, void *, void *, void *);
void Task_Kill(void *);
void sub_081F2BB0(void);
void SingletonSlot7_DestroyImpactSprite(void);

void *SingletonSlot7_CreateImpactSprite(void *a, void *b, void *c)
{
    void *r = Task_Create(10, 0x68);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081F2BB0, SingletonSlot7_DestroyImpactSprite);
        if (TimedSprite_Init(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
