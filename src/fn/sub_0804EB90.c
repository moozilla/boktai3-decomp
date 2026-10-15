#include "global.h"
#define AreaEffect_Create sub_0804EB90

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void sub_0804EA54(void);
void sub_0804EAFC(void);
s32 sub_0804EB60(void *, s32, s32);

void *AreaEffect_Create(s32 a, s32 b)
{
    void *p = Task_Create(9, 0xAB4);
    if (p) {
        Task_SetCallbacks(p, sub_0804EA54, sub_0804EAFC);
        if (sub_0804EB60(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
