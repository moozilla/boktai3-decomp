#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080E87C4(u8 *);
void sub_080E87B8(u8 *);
void sub_08219D38(u8 *);

void sub_080E986C(void)
{
    u8 *p = sub_08219C40(0x664);
    if (p != 0) {
        sub_08219DD8(p, 0x664);
        if (sub_080E87C4(p) < 0) {
            sub_080E87B8(p);
            sub_08219D38(p);
        }
    }
}
