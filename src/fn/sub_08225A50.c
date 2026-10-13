#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 sub_08225A3C(void *, u32, u32);
void sub_08225A2C(void);
void sub_08225A30(void);
extern void *gUnk_030025F8;
void *sub_08225A50(u32 a, u32 b)
{
    void *p;
    if (gUnk_030025F8 != 0)
        return gUnk_030025F8;
    p = Task_Create(2, 0x20);
    if (p) {
        Task_SetCallbacks(p, sub_08225A2C, sub_08225A30);
        if (sub_08225A3C(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
