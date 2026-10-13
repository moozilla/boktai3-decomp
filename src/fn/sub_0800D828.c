#include "global.h"

extern void *gUnk_0200046C;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0800D800(void *, void *);
void Task_Kill(void *);
void FallingObjectManager_Update(void);
void sub_0800D7D4(void);

void *sub_0800D828(void *a)
{
    void *p;
    if (gUnk_0200046C != 0)
        return gUnk_0200046C;
    p = Task_Create(8, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, FallingObjectManager_Update, sub_0800D7D4);
        if (sub_0800D800(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
