#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081B74A8(void *);
void sub_0821A0C0(void *);
void sub_081B744C(void);
void sub_081B748C(void);

void *sub_081B74D0(void)
{
    void *r = sub_08219FBC(9, 0x27c);
    if (r != 0) {
        sub_0821A04C(r, sub_081B744C, sub_081B748C);
        if (sub_081B74A8(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
