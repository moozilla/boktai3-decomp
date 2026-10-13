#include "global.h"
extern u32 gUnk_02000094;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08018600(void *);
void Task_Kill(void *);
void sub_080185B8(void);
void sub_080185F4(void);

void *sub_08018618(void)
{
    void *p;
    if (gUnk_02000094 != 0)
        return (void *)gUnk_02000094;
    p = Task_Create(0xc, 0x39c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080185B8, sub_080185F4);
        if (sub_08018600(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
