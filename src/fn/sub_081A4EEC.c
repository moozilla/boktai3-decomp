#include "global.h"

extern void *gUnk_02000244;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081A4DF0(void *, s32, s32, s32);
void Task_Kill(void *);
void sub_081A4D8C(void);
void sub_081A4DB4(void);

void *sub_081A4EEC(s32 a, s32 b, s32 c)
{
    void *r;
    if (gUnk_02000244 != 0)
        return gUnk_02000244;
    gUnk_02000244 = Task_Create(8, 0xEE4);
    r = gUnk_02000244;
    if (r != 0) {
        Task_SetCallbacks(r, sub_081A4D8C, sub_081A4DB4);
        if (sub_081A4DF0(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
