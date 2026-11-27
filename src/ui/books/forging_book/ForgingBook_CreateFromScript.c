#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 ForgingBook_InitializePageFromScript(void *, u32);
void CollectionBook_UpdateMenu(void);
void sub_08213B50(void);
void *ForgingBook_CreateFromScript(u32 a)
{
    void *p = Task_Create(8, 0x724);
    if (p) {
        Task_SetCallbacks(p, CollectionBook_UpdateMenu, sub_08213B50);
        if (ForgingBook_InitializePageFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
