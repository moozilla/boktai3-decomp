#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_08099B44(void *);
void sub_08099AAC(void *);

void sub_0809ADD4(void)
{
    void *m = sub_08219C40(0x62c);
    if (m != 0) {
        sub_08219DD8(m, 0x62c);
        if (sub_08099B44(m) < 0) {
            sub_08099AAC(m);
            sub_08219D38(m);
        }
    }
}
