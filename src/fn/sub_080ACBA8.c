#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_080ABDCC(void *);
void sub_080ABD30(void *);

void sub_080ACBA8(void)
{
    void *m = sub_08219C40(0x934);
    if (m != 0) {
        sub_08219DD8(m, 0x934);
        if (sub_080ABDCC(m) < 0) {
            sub_080ABD30(m);
            sub_08219D38(m);
        }
    }
}
