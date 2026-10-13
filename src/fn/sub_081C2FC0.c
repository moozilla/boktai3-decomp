#include "global.h"
extern void *gUnk_02000264;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C2D68(void *, void *);
void Task_Kill(void *);
void sub_081C2D0C(void *);
void sub_081C2D34(void *);
void *sub_081C2FC0(void *a)
{
    void *p;
    if (gUnk_02000264 != 0)
        return gUnk_02000264;
    p = gUnk_02000264 = Task_Create(8, 0x780);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C2D0C, sub_081C2D34);
        if (sub_081C2D68(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
