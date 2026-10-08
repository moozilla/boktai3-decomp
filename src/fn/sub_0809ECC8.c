#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_0809DD78(void *);
void sub_0809DCE4(void *);

void sub_0809ECC8(void)
{
    void *m = sub_08219C40(0x44c);
    if (m != 0) {
        sub_08219DD8(m, 0x44c);
        if (sub_0809DD78(m) < 0) {
            sub_0809DCE4(m);
            sub_08219D38(m);
        }
    }
}
