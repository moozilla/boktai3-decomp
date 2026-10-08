#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_080A1440(void *);
void sub_080A13AC(void *);

void sub_080A2010(void)
{
    void *m = sub_08219C40(0x43c);
    if (m != 0) {
        sub_08219DD8(m, 0x43c);
        if (sub_080A1440(m) < 0) {
            sub_080A13AC(m);
            sub_08219D38(m);
        }
    }
}
