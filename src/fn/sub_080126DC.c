#include "global.h"

extern void *gUnk_02000050;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08012684(void *);
void Task_Kill(void *);
void sub_08012630(void);
void sub_08012658(void);

void *sub_080126DC(void)
{
    void *p;
    if (gUnk_02000050 != 0)
        return gUnk_02000050;
    p = Task_Create(8, 0x538);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08012630, sub_08012658);
        if (sub_08012684(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
