#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_080575A8(void *, void *, void *);
void Task_Kill(void *);
void sub_08057574(void);
void sub_0805759C(void);
extern void *gUnk_0200049C;

void *sub_080575F8(void *a, void *b)
{
    void *r = gUnk_0200049C;
    void *q;
    if (r == 0) {
        q = Task_Create(8, 0x20);
        if (q != 0) {
            Task_SetCallbacks(q, sub_08057574, sub_0805759C);
            if (sub_080575A8(q, a, b) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
