#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_080C3260(void *);
void sub_080C3240(void *);

void sub_080C44AC(void)
{
    void *m = sub_08219C40(0x74c);
    if (m != 0) {
        sub_08219DD8(m, 0x74c);
        if (sub_080C3260(m) < 0) {
            sub_080C3240(m);
            sub_08219D38(m);
        }
    }
}
