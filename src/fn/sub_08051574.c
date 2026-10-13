#include "global.h"

extern void *gUnk_02000114;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_080513B8(void *, void *);
void Task_Kill(void *);
void sub_08051224(void);
void sub_08051390(void);

void *sub_08051574(void *a)
{
    void *p;
    if (gUnk_02000114 != 0)
        return gUnk_02000114;
    p = Task_Create(9, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08051224, sub_08051390);
        if (sub_080513B8(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
