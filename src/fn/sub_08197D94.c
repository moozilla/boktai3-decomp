#include "global.h"
#include "engine.h"

void *sub_0819792C(void *, void *, u32, s32, s32);
void sub_081979B4(void *, s32);
void sub_08197C98(void *);
void sub_08197A0C(void *);

void sub_08197D94(void *sprite, s32 count, s32 start)
{
    void *task = sub_0819792C(NULL, sprite, 0, count, start);
    if (task) {
        sub_081979B4(task, start);
        Task_SetCallbacks(task, sub_08197C98, sub_08197A0C);
    }
}
