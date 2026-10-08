#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_080A4AFC(void *);
void sub_080A4AA0(void *);

void sub_080A5B54(void)
{
    void *m = sub_08219C40(0x65c);
    if (m != 0) {
        sub_08219DD8(m, 0x65c);
        if (sub_080A4AFC(m) < 0) {
            sub_080A4AA0(m);
            sub_08219D38(m);
        }
    }
}
