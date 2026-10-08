#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_08090FEC(void *);
void sub_08090F58(void *);

void sub_08091E1C(void)
{
    void *m = sub_08219C40(0x40c);
    if (m != 0) {
        sub_08219DD8(m, 0x40c);
        if (sub_08090FEC(m) < 0) {
            sub_08090F58(m);
            sub_08219D38(m);
        }
    }
}
