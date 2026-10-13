#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0815F77C(void *, void *, void *);
void Task_Kill(void *);
void sub_0815F76C(void);
void sub_0815F770(void);
extern void *gUnk_02000598;

void *sub_0815F790(void *a, void *b)
{
    void *r = gUnk_02000598;
    void *q;
    if (r == 0) {
        q = Task_Create(2, 0x20);
        if (q != 0) {
            Task_SetCallbacks(q, sub_0815F76C, sub_0815F770);
            if (sub_0815F77C(q, a, b) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
