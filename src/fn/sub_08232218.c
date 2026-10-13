#include "global.h"

extern void *gUnk_0200005C;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_082321E0(void *, void *);
void Task_Kill(void *);
void sub_082321A4(void);
void sub_082321D4(void);

void *sub_08232218(void *a)
{
    void *p;
    if (gUnk_0200005C != 0)
        return gUnk_0200005C;
    p = Task_Create(9, 0x24);
    if (p != 0) {
        Task_SetCallbacks(p, sub_082321A4, sub_082321D4);
        if (sub_082321E0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
