#include "global.h"
#include "engine.h"

void *sub_0819792C(void *, void *, u32, s32, s32);
void sub_081979B4(void *, s32);
void sub_08197B7C(void *);
void sub_08197A0C(void *);

void sub_08197C20(void *anim, u32 hold, s32 count)
{
    void *task = sub_0819792C(anim, NULL, hold, count, 0);
    if (task) {
        sub_081979B4(task, 0);
        Task_SetCallbacks(task, sub_08197B7C, sub_08197A0C);
    }
}
