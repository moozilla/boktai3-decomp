#include "global.h"
#define AreaEffect_CreateLink sub_0804EF30

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void sub_0804EE94(void);
void sub_0804EED0(void);
s32 sub_0804EF0C(void *, s32, s32);

void *AreaEffect_CreateLink(s32 a, s32 b)
{
    void *p = Task_Create(9, 0xAB4);
    if (p) {
        Task_SetCallbacks(p, sub_0804EE94, sub_0804EED0);
        if (sub_0804EF0C(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
