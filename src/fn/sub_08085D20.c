#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_08084F44(void *);
void sub_08084EB0(void *);

void sub_08085D20(void)
{
    void *m = sub_08219C40(0x43c);
    if (m != 0) {
        sub_08219DD8(m, 0x43c);
        if (sub_08084F44(m) < 0) {
            sub_08084EB0(m);
            sub_08219D38(m);
        }
    }
}
