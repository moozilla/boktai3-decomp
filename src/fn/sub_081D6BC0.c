#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 MonsterBook_Init(void *, void *);
void Task_Kill(void *);
void sub_081D61C0(void);
void sub_081D68AC(void);
void *MonsterBook_Create(void *a)
{
    void *p = Task_Create(8, 0xf20);
    if (p) {
        Task_SetCallbacks(p, sub_081D61C0, sub_081D68AC);
        if (MonsterBook_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
