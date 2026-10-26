#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 ForgingBook_InitializeFromScript(void *, u32);
void sub_0821290C(void);
void ForgingBook_OnTaskDestroy(void);
void *ForgingBook_Create(u32 a)
{
    void *p = Task_Create(8, 0xbf0);
    if (p) {
        Task_SetCallbacks(p, sub_0821290C, ForgingBook_OnTaskDestroy);
        if (ForgingBook_InitializeFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gForgingBook = p;
    return p;
}
