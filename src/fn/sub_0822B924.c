#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0822B8EC(void *, u32, u32);
void Task_Kill(void *);
void sub_0822B8D4(void);
void sub_0822B8E0(void);

void *sub_0822B924(u32 a, u32 b)
{
    void *p = Task_Create(5, 0xD8);
    if (p) {
        Task_SetCallbacks(p, sub_0822B8D4, sub_0822B8E0);
        if (sub_0822B8EC(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
