#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_08094764(void *);
void sub_080946D0(void *);

void sub_08095540(void)
{
    void *m = sub_08219C40(0x490);
    if (m != 0) {
        sub_08219DD8(m, 0x490);
        if (sub_08094764(m) < 0) {
            sub_080946D0(m);
            sub_08219D38(m);
        }
    }
}
