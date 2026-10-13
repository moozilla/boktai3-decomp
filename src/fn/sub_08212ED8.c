#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 sub_08212D84(void *, u32);
void sub_0821290C(void);
void sub_08212D64(void);
extern void *gUnk_02000608;
void *sub_08212ED8(u32 a)
{
    void *p = Task_Create(8, 0xbf0);
    if (p) {
        Task_SetCallbacks(p, sub_0821290C, sub_08212D64);
        if (sub_08212D84(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_02000608 = p;
    return p;
}
