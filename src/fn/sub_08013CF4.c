#include "global.h"
extern u32 gUnk_02000470;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08013CE4(void *);
void Task_Kill(void *);
void sub_08013CA4(void);
void sub_08013CA8(void);

void *sub_08013CF4(void)
{
    void *p;
    if (gUnk_02000470 != 0)
        return (void *)gUnk_02000470;
    p = Task_Create(9, 0x1c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08013CA4, sub_08013CA8);
        if (sub_08013CE4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
